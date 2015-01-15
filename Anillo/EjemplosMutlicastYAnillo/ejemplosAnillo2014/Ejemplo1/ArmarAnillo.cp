/* 
 * File:   Broker.cpp
 * Author: knoppix
 * 
 * Created on June 24, 2014, 8:39 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>
#include "ArmarAnillo.h"
#include "Config.h"

ArmarAnillo::ArmarAnillo(bool master, unsigned short udp_port, std::string mcast_address, unsigned short mcast_port, int esperaMaxima) {
	this->first = master;
	this->udp_port = udp_port;
	this->multicast_address = mcast_address;
	this->multicast_port = mcast_port;
	this->esperaMaxima = esperaMaxima;
}

void ArmarAnillo::crearConexionesTCP(unsigned short tcp_port, Socket * &anterior, Socket * &siguiente) {

	Socket * anteriorMaster;
	siguiente = new Socket("siguiente", true);
	anteriorMaster = new Socket("anteriorMaster", true);
	anterior = new Socket("anterior", true);
	if (this->first) {
		sleep(2); //esperar a que el ultimo del anillo que recien me conecto empieze a escuchar
		siguiente->active(inet_ntoa(ip_addr_siguiente), tcp_port);

		anteriorMaster->passive(tcp_port);
		anterior = anteriorMaster->doAccept();
	} else {
		anteriorMaster->passive(tcp_port);
		anterior = anteriorMaster->doAccept();

		siguiente->active(inet_ntoa(ip_addr_siguiente), tcp_port);
	}
}

struct in_addr ArmarAnillo::crearAnillo(void) {
	/*signals for timeout*/
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &handle_signal;
	sigaction(SIGALRM, &sa, NULL);

	/*variables*/
	int bytes, fd_multicast, fd_remote_ant, fd_remote_sig, sigFlag = 0;
	int waitingCounter = 0;
	net_message msg;
	struct ip_mreq mreq;
	socklen_t sockaddrlen;
	host_info local_host_info, remote_host_info_ant, remote_host_info_sig,
			master_host_info;
	char* direccion, ip_addr[255];
	struct in_addr ip_addr_local, ip_addr_next;

	ip_addr_local = getIPAddr();
	strcpy(ip_addr, inet_ntoa(ip_addr_local));

	/*SET MULTICAST ADDRESS*/

	llenaStructMulticastSender(&(this->multicast_addr));
	mreq.imr_multiaddr.s_addr = inet_addr(this->multicast_address.c_str());
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	/*SET LOCAL ADDRESS*/
	memset(&local_address, 0, sizeof(local_address));
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);
	local_address.sin_port = htons(this->udp_port);
	local_host_info.address = local_address;

	//FIN INICIALIZACIONES
	//FIN INICIALIZACIONES

	//CREO Y BINDEO SOCKET PARA COMUNICARME CON EL BROKER ANTERIOR
	if ((fd_remote_ant = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");

		exit(EXIT_FAILURE);
	}

	if (bind(fd_remote_ant, (struct sockaddr *) &local_address, sizeof(local_address)) < 0) {
		perror("bind");

		exit(1);
	}

	//PROCESO DE LISTENERS
	if (!this->first) {
		//DECLARO Y BINDEO SOCKET MULTICAST
		u_int yes = 1;
		if ((fd_multicast = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("socket");

			exit(1);
		}
		if (setsockopt(fd_multicast, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			perror("Reusing ADDR failed");

			exit(1);
		}
		if (bind(fd_multicast, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr)) < 0) {
			perror("bind");

			exit(1);
		}
		if (setsockopt(fd_multicast, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
			perror("setsockopt");

			exit(1);
		}

		//PROCESO ENVIAR MULTICAST Y ESPERAR PRIMERA RESPUESTA
		do {
			/**********************RECEIVE FROM MULTICAST**********************/
			sigFlag = 0;
			sockaddrlen = sizeof(multicast_addr);
			//recibo mensaje multicast
			if ((bytes = recvfrom(fd_multicast, &msg, sizeof(msg), 0, (struct sockaddr *) &multicast_addr, &sockaddrlen)) < 0) {
				perror("recvfrom");

				exit(1);
			}
			if (msg.action != ArmarAnilloProtocol::SET_UP) {
				perror("Multicast unexpected message\n");

				exit(1);
			}

			//GUARDO INFO DEL HOST ANTERIOR
			remote_host_info_ant.address.sin_addr.s_addr = inet_addr(msg.dir); //atoi(msg.dir);
			remote_host_info_ant.address.sin_port = msg.port;
			remote_host_info_ant.address.sin_family = AF_INET;

			/**********************SEND CONNECT TO P2P**********************/

			memset(&remote_address_ant, 0, sizeof(remote_address_ant));
			remote_address_ant.sin_addr = remote_host_info_ant.address.sin_addr;
			remote_address_ant.sin_family = remote_host_info_ant.address.sin_family;
			remote_address_ant.sin_port = remote_host_info_ant.address.sin_port;

			//CREO MENSAJE PARA EL HOST ANTERIOR CON PEDIDO DE UNIRME

			strncpy(msg.dir, ip_addr, 16);
			msg.port = local_host_info.address.sin_port;
			msg.action = ArmarAnilloProtocol::CONNECT;

			//MANDO MENSAJE AL HOST ANTERIOR
			sockaddrlen = sizeof(remote_address_ant);
			if ((bytes = sendto(fd_remote_ant, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_ant, sockaddrlen)) < 0) {
				perror("sendto");
				exit(1);
			}

			/**********************RECEIVE CONFIRM FROM P2P**********************/

			//LEVANTAR TIMEOUT!!!!
			alarm(3);

			//ESPERO MENSAJE DE CONFIRMACION DEL HOST ANTERIOR, SI NO LLEGA DA TIMEOUT Y VUELVO A ESCUCHAR AL PRINCIPIO
			sockaddrlen = sizeof(remote_address_ant);
			if ((bytes = recvfrom(fd_remote_ant, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_ant, &sockaddrlen)) < 0) {
				if (errno == EBADF || errno == EINTR) {
					sigFlag = 1; //SIGNAL FLAG, VUELVO A ESCUCHAR
				} else { //ERROR DE COMUNICACION TIRO TODO A LA MIERDA
					perror("recvfrom\n");
					printf("%d\n", errno);
					exit(1);
				}
			} else {
				alarm(0);
			}

			if (!sigFlag) { //SI NO ME DIO TIMEOUT RECIBI CONFIRM CON DATOS DEL MASTER
				//CHEQUEO TIPO DE MENSAJE CORRECTO
				if (msg.action != ArmarAnilloProtocol::CONFIRM) {
					perror("Multicast unexpected message\n");

					exit(EXIT_FAILURE);
				}
				//GUARDO DATOS DEL MASTER
				master_host_info.address.sin_addr.s_addr = inet_addr(msg.dir);
				master_host_info.address.sin_family = AF_INET;
				master_host_info.address.sin_port = msg.port;
			}
		} while (sigFlag);

	} else {
		master_host_info = local_host_info; //LA INFO DEL MASTER ES LA INFO PROPIA PARA EL PRIMER BROKER
		master_host_info.address.sin_addr.s_addr = inet_addr(ip_addr);
		master_host_info.address.sin_port = local_address.sin_port;
	}

	//**********************SENDER Y RESENDER**********************
	//**********************SENDER Y RESENDER**********************
	//**********************SENDER Y RESENDER**********************

	//CREO Y BINDEO SOCKET DE HOST SIGUIENTE PARA MANEJAR PEDIDOS Y DEVOLVER CONFIRMACION
	if ((fd_remote_sig = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");

		exit(EXIT_FAILURE);
	}

	local_address.sin_port = htons(udp_port + 100);
	local_host_info.address.sin_port = local_address.sin_port;
	if (bind(fd_remote_sig, (struct sockaddr *) &local_address, sizeof(local_address)) < 0) {
		perror("bind");

		exit(1);
	}

	/**********************SEND TO MULTICAST**********************/

	//LIMPIO STRUCTURA MULTICAST POR SI QUEDO CON VALORES DEL ANTERIOR Y VUELVO A LLENAR
	llenaStructMulticastSender(&multicast_addr);

	if ((fd_multicast = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");

		exit(1);
	}

	/*
	 *REPITO MANDAR MULTICAST Y ESPERAR RESPUESTAS TRES VECES SI NO SOY EL PRIMERO, SI NADIE RESPONDE
	 *MANDO MIS DATOS AL PRIMERO.
	 */

	do {
		sigFlag = 0;

		//PONGO MIS DATOS Y MANDO MULTICAST
		strncpy(msg.dir, ip_addr, 16);
		msg.port = local_host_info.address.sin_port;
		msg.action = ArmarAnilloProtocol::SET_UP;

		if (sendto(fd_multicast, &msg, sizeof(msg), 0, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr)) < 0) {
			perror("sendto");

			exit(1);
		}

		/**********************RECEIVE CONNECT FROM P2P**********************/

		//LEVANTAR TIMEOUT!!!!
		alarm(3);

		//RECIBO DATOS DEL PRIMERO QUE MANDE
		sockaddrlen = sizeof(remote_address_sig);
		if ((bytes = recvfrom(fd_remote_sig, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_sig, &sockaddrlen)) < 0) {
			if (errno == EBADF || errno == EINTR) {
				//perror("Señal corta rcv");
				sigFlag = 1; //MARCO QUE HUBO TIMEOUT
				if (first) { //AL PRIMER TIMEOUT EL PRIMERO SE DA DE BAJA
					exit(EXIT_FAILURE);
				}
			} else { //ERROR DE COMUNICACION, RAJO
				perror("recvfrom");
				exit(1);
			}
		} else {
			alarm(0);
		}
		waitingCounter++;
	} while (sigFlag && !first && waitingCounter <= this->esperaMaxima); //MIENTRAS HAYA TIMEOUT, NO SEA EL PRIMERO Y SEA 3RA VEZ

	if (waitingCounter > this->esperaMaxima && !first) { //NO MASTER Y ESPERAR TRES VECES, SIGUIENTE ES MASTER, MANDO DATOS
		//conectar con master y cerrar circulo

		//METO DATOS DEL MASTER EN EL SIGUIENTE

		remote_host_info_sig.address.sin_addr = master_host_info.address.sin_addr;
		remote_host_info_sig.address.sin_family = master_host_info.address.sin_family;
		remote_host_info_sig.address.sin_port = master_host_info.address.sin_port;
		memset(&remote_address_sig, 0, sizeof(remote_address_sig));
		remote_address_sig.sin_addr = master_host_info.address.sin_addr;
		remote_address_sig.sin_family = master_host_info.address.sin_family;
		remote_address_sig.sin_port = master_host_info.address.sin_port;

		direccion = inet_ntoa(remote_host_info_sig.address.sin_addr);
		strncpy(msg.dir, direccion, 16);

		ip_addr_next.s_addr = inet_addr(msg.dir);

		//ARMO MENSAJE CON MIS DATOS Y MANDO AL MASTER

		strncpy(msg.dir, ip_addr, 16);
		msg.port = local_host_info.address.sin_port;
		msg.action = ArmarAnilloProtocol::CONFIRM;

		if ((bytes = sendto(fd_remote_sig, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_sig, sizeof(remote_address_sig))) < 0) {
			perror("sendto");

			exit(1);
		}
		ip_addr_siguiente = ip_addr_next;
		return (ip_addr_next);
	}

	//NO SOY MASTER Y ME LLEGO ALGO
	if (msg.action != ArmarAnilloProtocol::CONNECT) {
		perror("Multicast unexpected message\n");

		exit(1);
	}

	//GUARDO DATOS DEL SIGUIENTE
	remote_host_info_sig.address.sin_addr.s_addr = inet_addr(msg.dir);
	remote_host_info_sig.address.sin_family = AF_INET;
	remote_host_info_sig.address.sin_port = msg.port;

	ip_addr_next.s_addr = inet_addr(msg.dir);

	/**********************SEND CONFIRM TO P2P**********************/

	//GUARDO DATOS DEL SIGUIENTE EN SOCKET
	memset(&remote_address_sig, 0, sizeof(remote_address_sig));
	remote_address_sig.sin_addr = remote_host_info_sig.address.sin_addr;
	remote_address_sig.sin_family = remote_host_info_sig.address.sin_family;
	remote_address_sig.sin_port = remote_host_info_sig.address.sin_port;

	//ARMO MENSAJE CON DATOS DEL MASTER Y ENVIO
	direccion = inet_ntoa(master_host_info.address.sin_addr);
	strncpy(msg.dir, direccion, 16);
	msg.port = master_host_info.address.sin_port;
	msg.action = ArmarAnilloProtocol::CONFIRM;

	sleep(1);
	if ((bytes = sendto(fd_remote_sig, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_sig, sizeof(remote_address_sig))) < 0) {
		perror("sendto");

		exit(1);
	}

	if (first) {              //SI SOY MASTER ME QUEDO ESCUCHANDO PARA EL ULTIMO

		sockaddrlen = sizeof(remote_address_ant);
		if ((bytes = recvfrom(fd_remote_ant, &msg, sizeof(msg), 0, (struct sockaddr *) &remote_address_ant, &sockaddrlen)) < 0) {
			perror("recvfrom\n");
			printf("%d\n", errno);

			exit(1);
		}
		//GUARDO DATOS DEL ANTERIOR Y TERMINO
		remote_host_info_ant.address.sin_addr.s_addr = inet_addr(msg.dir);
		remote_host_info_ant.address.sin_family = AF_INET;
		remote_host_info_ant.address.sin_port = msg.port;
	}
	ip_addr_siguiente = ip_addr_next;
	return (ip_addr_next);
}

void ArmarAnillo::handle_signal(int signal) {
	// Prevent unused parameter warning
	signal %= 2;
}

struct in_addr ArmarAnillo::getIPAddr(void) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;
	struct in_addr address;
	bzero(&address, sizeof(address));
	getifaddrs(&ifAddrStruct);
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (strcmp(addressBuffer, "127.0.0.1")) {
				address = *(struct in_addr *) tmpAddrPtr;
				//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
				if (ifAddrStruct != NULL)
					freeifaddrs(ifAddrStruct);
				return address;
			}
		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
	return address;
}

void ArmarAnillo::llenaStructMulticastSender(struct sockaddr_in *multicast_address) {
	memset(multicast_address, 0, sizeof(struct sockaddr_in));
	multicast_address->sin_family = AF_INET;
	multicast_address->sin_addr.s_addr = inet_addr(this->multicast_address.c_str()); //htonl(INADDR_ANY);
	multicast_address->sin_port = htons(this->multicast_port);
}
