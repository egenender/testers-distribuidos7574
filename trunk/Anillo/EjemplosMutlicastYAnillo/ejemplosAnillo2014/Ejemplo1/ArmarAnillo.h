/* 
 * File:   Broker.h
 * Author: knoppix
 *
 * Created on June 24, 2014, 8:39 PM
 */

#ifndef BROKER_H
#define	BROKER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>
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
#include <netdb.h>
#include "Socket.h"

class ArmarAnillo {
	public:
		ArmarAnillo(bool master, unsigned short udp_port, std::string mcast_address, unsigned short mcast_port, int esperaMaxima = 2);
		struct in_addr crearAnillo(void);
		static void handle_signal(int signal);
		struct in_addr getIPAddr(void);
		void crearConexionesTCP(unsigned short tcp_port, Socket * &anterior, Socket * &siguiente);
	private:
		enum class ArmarAnilloProtocol {
			SET_UP, CONNECT, CONFIRM
		};

		typedef struct {
				char dir[16];
				unsigned short port;
				ArmarAnilloProtocol action;
		} net_message;

		typedef struct {
				struct sockaddr_in address;
				int id;
		} host_info;

		void llenaStructMulticastSender(struct sockaddr_in *multicast_address);
		int esperaMaxima;
		bool first;
		unsigned short udp_port, multicast_port;
		std::string multicast_address;
		struct sockaddr_in remote_address_ant, remote_address_sig,
				multicast_addr, local_address;
		struct in_addr ip_addr_siguiente;
};

#endif	/* BROKER_H */

