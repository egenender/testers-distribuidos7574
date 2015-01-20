#include <sys/wait.h>
#include "inet.h"
#include "tcpopact.cpp"
#include "tcpoppas.cpp"
#include "enviar.cpp"
#include "recibir.cpp"
#include "siguiente.h"

#define MSGBUFSIZE 256

char* imprimirCodigo(int codigo);
void sig_handler(int sig);
extern int tcpopact(char*, int);
extern int tcpoppas(int);
extern int enviar (int, void*, size_t);
extern int recibir (int, void*, size_t);

main(int argc, char *argv[])
{
    key_t key = ftok("/tmp/buchwaldipcs", SHM_VERSION);
    int shmversion = shmget(key, sizeof(uint64_t) , 0660 | IPC_CREAT);
    uint64_t* id = (uint64_t*)shmat(shmversion, NULL, 0);   
    
    char mostrar[300];
    
    sprintf(mostrar, "[BROKER] --> ID: %lu\t| pid: %d\n", *id, getpid());
    write(fileno(stdout), mostrar, strlen(mostrar));
    
    struct sigaction sa; 
    int fdSiguiente;
    int fdAnterior;
    char ipBrokerSiguiente[16];
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&sig_handler; 
    sigaction(SIGUSR1, &sa, NULL); 
        
    MsgInvitacion_t msgInvitacion; 
    MsgMulticast_t msgMulticast;
    MsgLider_t msgLider;
    
    char dirMaster[50];
    char puerto[50];
    int portTcp;
    char dirMulticast[50];
    char puertoMulticast[50];
    int portMulticast;
    char puertoUdp[50];
    int portUdp;
   	
	
    FILE * fdConfig = fopen("Anillo/config", "r");
    if(fdConfig==NULL){
        perror("fopen");
        exit(1);
    }
    //Tomo del archivo config los valores de direccion del master, multicast y puertos
    fgets(mostrar,sizeof(mostrar),fdConfig); //Leo la primer linea y la ignoro
    //Leo la direccion del master y el puerto por el que se hacen los P2P
    fscanf(fdConfig, "%s", dirMaster);
    fscanf(fdConfig, "%s\n", puerto);
    fgets(mostrar,sizeof(mostrar),fdConfig); //Leo la proxima linea y la ignoro
    //fgets(mostrar,sizeof(mostrar),fdConfig); //Leo la proxima linea y la ignoro
    //Leo Direccion de Multicast y puerto de multicast
    fscanf(fdConfig, "%s", dirMulticast);
    fscanf(fdConfig, "%s\n", puertoMulticast);
    fgets(mostrar,sizeof(mostrar),fdConfig); //Leo la proxima linea y la ignoro
    fscanf(fdConfig, "%s", puertoUdp);
    fclose(fdConfig);
    portTcp = atoi(puerto);
    portMulticast = atoi(puertoMulticast);
    portUdp = atoi(puertoUdp);
    
    sprintf(mostrar,"[BROKER] --> IP Master: %s - Puerto P2P: %d - IP Multicast: %s - Puerto Multicast: %d - Puerto UDP: %d\n",dirMaster,portTcp,dirMulticast,portMulticast,portUdp);
    write(fileno(stdout),mostrar,strlen(mostrar));
    
     struct sockaddr_in addrMulticast;
     struct sockaddr_in addr;
     int fdMulticast;
     int fd;
     int nbytes;
     int addrlen;
     
     struct ip_mreq mreq;
     char msgbuf[MSGBUFSIZE];

     char s_pid[15];
     pid_t childpid;
     u_int yes=1;         

     /* create what looks like an ordinary UDP socket */
	if ((fdMulticast=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket multicast");
		exit(1);
	}

    /**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fdMulticast,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
    }

    /*** END OF MODIFICATION TO ORIGINAL */
    /* set up destination address */
    memset(&addrMulticast,0,sizeof(addrMulticast));
    addrMulticast.sin_family=AF_INET;
    addrMulticast.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
    addrMulticast.sin_port=htons(portMulticast);
     
    /* bind to receive address */
    if (bind(fdMulticast,(struct sockaddr *) &addrMulticast,sizeof(addrMulticast)) < 0) {
		perror("bind multicast");
		exit(1);
    }
     
    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr=inet_addr(dirMulticast);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(fdMulticast,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}
     
	//Socket para comunicacion UDP con brokers
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(1);
	}
     
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
		perror("Reusing ADDR failed");
		exit(1);
	}
     
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY);
     addr.sin_port=htons(portUdp);
     
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  perror("bind");
	  exit(1);
     }

     /* now just enter a read-print loop */
    do {
         //Esperando mensaje Multicast
	  addrlen=sizeof(addrMulticast);
	  
          if ((nbytes=recvfrom(fdMulticast,&msgMulticast,sizeof(MsgMulticast_t),0,(struct sockaddr *) &addrMulticast,&addrlen)) < 0) {
	       perror("recvfrom");
	       exit(1);
	  }
          sprintf(mostrar,"[RECIBIDO] <-- %s\t%s\n",inet_ntoa(addrMulticast.sin_addr), imprimirCodigo(msgMulticast.tipo));
          write(fileno(stdout),mostrar,strlen(mostrar));

          
          //Armando respuesta para enviar al broker
          msgInvitacion.tipo=UNIRME;
          addr.sin_addr = addrMulticast.sin_addr;
          addrlen = sizeof(addr);
          
        if (sendto(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr,(socklen_t)addrlen) < 0) {
          perror("sendto\n");
          exit(1);
        }
          
        sprintf(mostrar,"[ENVIADO] --> %s\t%s\n",inet_ntoa(addr.sin_addr),imprimirCodigo(msgInvitacion.tipo));
        write(fileno(stdout),mostrar,strlen(mostrar));
        
         
        sprintf(s_pid, "%d", getpid());
        
        childpid = fork();
        if(childpid<0) {
            perror("Error en el fork\n");
        } else if (childpid==0) {
            execlp("./Anillo/timeout", "timeout", s_pid, (char*)0);
            perror("Error al lanzar el proceso timeout");
            exit(1);
        }
        
        nbytes=recvfrom(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr, &addrlen);
        if(nbytes<0) {
          if(errno==EINTR) {
            sprintf(mostrar,"[BROKER] --> Timeout expirado\n");
            write(fileno(stdout), mostrar, strlen(mostrar));
          }
          msgInvitacion.tipo=NO_UNIDO;
          
        } else {
			sprintf(mostrar,"kill -9 %d",childpid);
			system(mostrar);
			wait(NULL);
			write(fileno(stdout),mostrar,strlen(mostrar));
        }
        
        
        sprintf(mostrar, "\n[RECIBIDO] <-- %s\t%s\n", inet_ntoa(addr.sin_addr),imprimirCodigo(msgInvitacion.tipo));
        write(fileno(stdout), mostrar, strlen(mostrar));
        
    } while(msgInvitacion.tipo!=UNIDO);
    

    
    sprintf(mostrar, "[RECIBIDO] <-- IP DEL MASTER: %s\n", msgInvitacion.direccionMaster);
    write(fileno(stdout), mostrar, strlen(mostrar));
    char ipMaster[15];
    strcpy(ipMaster, msgInvitacion.direccionMaster);
     
    //========================================
    
    sprintf(mostrar, "::::::: ENVIO DE BROADCAST :::::::\n");
    write(fileno(stdout), mostrar, strlen(mostrar));
    /* set up destination address */
    memset(&addrMulticast,0,sizeof(addrMulticast));
    addrMulticast.sin_family=AF_INET;
    addrMulticast.sin_addr.s_addr=inet_addr(dirMulticast);
    addrMulticast.sin_port=htons(portMulticast);
     
    
    //Socket para comunicacion UDP entre brokers
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(portUdp);
    addrlen = sizeof(addr);
   
    /* now just sendto() our destination! */
       addrlen = sizeof(addrMulticast);
       
       msgMulticast.tipo=INVITACION;
       
       if (sendto(fdMulticast,&msgMulticast,sizeof(MsgMulticast_t),0,(struct sockaddr *) &addrMulticast,(socklen_t)addrlen) < 0) {
            perror("sendto\n");
            exit(1);
       }
       
       sprintf(mostrar,"[ENVIADO] Invitacion --> %s\n",inet_ntoa(addrMulticast.sin_addr));
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       sleep(1);
       
       sprintf(mostrar,"[BROKER] Esperando respuesta de algun broker\n");
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       
        childpid = fork();
        if(childpid<0) {
            perror("Error en el fork\n");
        } else if (childpid==0) {
            execlp("./Anillo/timeout", "timeout", s_pid, (char*)0);
            perror("Error al lanzar el proceso timeout");
            exit(1);
        }
        
        nbytes=recvfrom(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr, &addrlen);
        if(nbytes<0) {
          if(errno==EINTR) {
			    sprintf(mostrar,"[BROKER] --> Timeout expirado\n");
                write(fileno(stdout),mostrar,strlen(mostrar));
                sprintf(mostrar,"[BROKER] --> Cerrar Anillo\n");
                write(fileno(stdout),mostrar,strlen(mostrar));
                addr.sin_addr.s_addr = inet_addr(ipMaster);
                msgInvitacion.tipo=CERRAR;
                sleep(3);
                if (sendto(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr,(socklen_t)addrlen) < 0) {
                     perror("sendto\n");
                     exit(1);
                }

             sprintf(mostrar,"[ENVIADO] --> %s\t%s\n",inet_ntoa(addrMulticast.sin_addr),imprimirCodigo(CERRAR));
            write(fileno(stdout),mostrar,strlen(mostrar));
            strcpy(ipBrokerSiguiente, inet_ntoa(addr.sin_addr));
			set_siguiente(ipBrokerSiguiente);
          }        
        } else {
			sprintf(mostrar,"kill -9 %d",childpid);
			system(mostrar);
			wait(NULL);
			write(fileno(stdout),mostrar,strlen(mostrar));
		    sprintf(mostrar,"\n[RECIBIDO] <-- %s\t%s\n",inet_ntoa(addr.sin_addr), imprimirCodigo(msgInvitacion.tipo));
		    write(fileno(stdout),mostrar,strlen(mostrar));
            msgInvitacion.tipo=UNIDO;
            strcpy(msgInvitacion.direccionMaster, ipMaster);
             
            strcpy(ipBrokerSiguiente, inet_ntoa(addr.sin_addr));
            set_siguiente(ipBrokerSiguiente);
            sprintf(mostrar, "[BROKER] --> Ip del Broker siguiente: %s\n", ipBrokerSiguiente);
            write(fileno(stdout), mostrar, strlen(mostrar));
            if (sendto(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr,(socklen_t)addrlen) < 0) {
                 perror("sendto\n");
                 exit(1);
            }
        }
        
        fdAnterior=tcpoppas(portTcp);
        if(fdAnterior < 0){
            perror("tcpoppas");
            exit(1);
        }
        
	int newsockfd;
 	addrlen = sizeof (addr);
        newsockfd = accept(fdAnterior, (struct sockaddr *) &addr, &addrlen);
        if(newsockfd < 0){
            perror("accept");
            exit(1);
        }
        sleep(3);
        sprintf(mostrar, "[CONEXION ESTABLECIDA] --> Broker anterior %s\n", inet_ntoa(addr.sin_addr)); 
        write(fileno(stdout), mostrar, strlen(mostrar));
        
        fdSiguiente=tcpopact(ipBrokerSiguiente, portTcp);
        if(fdSiguiente < 0){
            perror("tcpopact");
            exit(1);
        }
        sprintf(mostrar, "[CONEXION ESTABLECIDA] --> Broker siguiente %s\n", ipBrokerSiguiente); 
        write(fileno(stdout), mostrar, strlen(mostrar));
        
	    msgLider.idBroker = *id; 
        msgLider.estado=DESCONOCIDO;

       
       do {
		   sprintf(mostrar,"[ESPERANDO] --> Recibir mensaje de: %s\n",inet_ntoa(addr.sin_addr));
           write(fileno(stdout), mostrar, strlen(mostrar));
           nbytes = recibir(newsockfd, &msgLider, sizeof(MsgLider_t));
	       if(nbytes < 0){
               sprintf(mostrar,"Error al recibir msgLider\n");
               write(fileno(stdout),mostrar,strlen(mostrar));
               exit(1);
           }else{
               sprintf(mostrar,"[RECIBIDO] <-- ID: %lu\t%s\n",msgLider.idBroker, imprimirCodigo(msgLider.estado));
               write(fileno(stdout),mostrar,strlen(mostrar));

               if((msgLider.idBroker > *id)&&(msgLider.estado!=FIN)){
				   msgLider.estado=DESCONOCIDO;
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
               }else if(msgLider.idBroker == *id){
                   msgLider.estado = FIN;
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
               }else if(msgLider.estado == FIN){
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
               }else if(msgLider.idBroker < *id){
				   msgLider.idBroker = *id;
				   msgLider.estado=DESCONOCIDO;
				   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
			   }
			   sprintf(mostrar,"[ENVIADO] --> id: %lu\t%s\n",msgLider.idBroker, imprimirCodigo(msgLider.estado));
			   write(fileno(stdout), mostrar, strlen(mostrar));
           }
       } while((msgLider.estado != LIDER) && (msgLider.estado!=FIN));
       
       sprintf(mostrar,"[BROKER] --> ID LIDER ENCONTRADO: %lu\t%s\n",msgLider.idBroker, imprimirCodigo(msgLider.estado));
       set_lider(msgLider.idBroker == *id);
       if (msgLider.idBroker == *id) printf("SOY LIDER\n");
	   write(fileno(stdout), mostrar, strlen(mostrar));
	   
	   sprintf(mostrar,"::::: QUEDA ESTABLECIDO EL ANILLO :::::\n");
	   write(fileno(stdout), mostrar, strlen(mostrar));
		
		key = ftok("/tmp/buchwaldipcs",SEM_ANILLO_FORMANDO);
		int semid = semget(key,1, IPC_CREAT| 0660);
		struct sembuf oper;
		oper.sem_num = 0;
		oper.sem_op = 1;
		oper.sem_flg = 0;
		semop(semid,&oper,1);
		
		shmdt((void*)id);
		execlp("./Anillo/listener", "listener", argv[1],(char*)0);
}

void sig_handler(int sig) {
    char mostrar[50]; 
    sprintf(mostrar,"Se recibio el timeout\n");
    write(fileno(stdout), mostrar, strlen(mostrar));
    wait(NULL);
}


char* imprimirCodigo(int codigo){
    char mostrar[50];
    switch(codigo){
        case INVITACION:
            return "INVITACION";
        case UNIDO:
            return "UNIDO";
        case UNIRME:
            return "UNIRME";
        case NO_UNIDO:
            return "NO_UNIDO";
        case CERRAR:
            return "CERRAR";
        case DESCONOCIDO:
            return "DESCONOCIDO";
        case LIDER:
            return "LIDER"; 
        case FIN:
            return "FIN";
        default:
            return "CODIGO NO VALIDO";
    }
}
