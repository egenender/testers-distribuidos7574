#include "inet.h"
#include "tcpopact.cpp"
#include "tcpoppas.cpp"
#include "enviar.cpp"
#include "recibir.cpp"
#include "siguiente.h"

extern int tcpopact(char *, int);
extern int tcpoppas(int);
extern int recibir(int, void *, size_t);
extern int enviar(int, void *, size_t);

char* imprimirCodigo(int codigo);

main(int argc, char *argv[])
{
    char mostrar[300];
    
    key_t key = ftok("/tmp/buchwaldipcs", SHM_VERSION);
    int shmversion = shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    int* id = (int*)shmat(shmversion, NULL, 0);   
    
    sprintf(mostrar,"[MASTER] --> ID: %d\t| pid: %d\n",*id, getpid());
    write(fileno(stdout),mostrar,strlen(mostrar));
    
    struct sockaddr_in addrMulticast;
    int fdMulticast;
     
    int fd;
    int fdSiguiente;
    int fdAnterior;
    int newsockfd;
    struct sockaddr_in addr;
    
    MsgMulticast_t msgMulticast; 
    MsgInvitacion_t msgInvitacion;
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
    fscanf(fdConfig, "%s",puertoUdp);
    fclose(fdConfig);
    portTcp = atoi(puerto);
    portMulticast = atoi(puertoMulticast);
    portUdp = atoi(puertoUdp);
    
    sprintf(mostrar,"[MASTER] --> IP Master: %s - Puerto P2P: %d - IP Multicast: %s - Puerto Multicast: %d - Puerto UDP: %d\n",dirMaster,portTcp,dirMulticast,portMulticast,portUdp);
    write(fileno(stdout),mostrar,strlen(mostrar));
    
    int addrlen;
    int nbytes;
    u_int yes = 1;
    
    char ipBrokerSiguiente[16];

    /* create what looks like an ordinary UDP socket */
    if ((fdMulticast=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
         perror("socket multicast");
	 exit(1);
    }

    /* set up destination address */
    memset(&addrMulticast,0,sizeof(addrMulticast));
    addrMulticast.sin_family=AF_INET;
    addrMulticast.sin_addr.s_addr=inet_addr(dirMulticast);
    addrMulticast.sin_port=htons(portMulticast);
     
    if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	 perror("socket");
	 exit(1);
    }
     
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
    }
    
    //Socket para comunicacion UDP entre brokers
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(portUdp);
    addrlen = sizeof(addr);
    if(bind(fd,(struct sockaddr *)&addr,addrlen) < 0){
        perror("bind\n");
        exit(1);
    }
    
    
    /* now just sendto() our destination! */
       addrlen = sizeof(addrMulticast);
       msgMulticast.tipo=INVITACION;
       
       //Envio multicast para ver si alguien quiere unirse
       if (sendto(fdMulticast,&msgMulticast,sizeof(MsgMulticast_t),0,(struct sockaddr *) &addrMulticast,(socklen_t)addrlen) < 0) {
            perror("sendto\n");
            exit(1);
       }
       sprintf(mostrar,"[ENVIADO] --> %s\t%s\n",inet_ntoa(addrMulticast.sin_addr), imprimirCodigo(INVITACION));
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       sleep(1);
       
       sprintf(mostrar,"[ESPERANDO] --> Respuesta de algun broker\n");
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       //Espero que alguno de los que se quiere unir me responda
       if((nbytes = recvfrom(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *)&addr,(socklen_t *)&addrlen)) < 0){
           perror("recvfrom\n");
           exit(1);
       }
       sprintf(mostrar,"[RECIBIDO] <-- %s\t%s\n",inet_ntoa(addr.sin_addr),imprimirCodigo(msgInvitacion.tipo));
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       //Aviso que me llegó su respuesta y puede unirse. Envio tambien la direccion del master para que el ultimo pueda cerrar el anillo
       msgInvitacion.tipo=UNIDO;
       strcpy(msgInvitacion.direccionMaster,dirMaster);
       if (sendto(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *) &addr,(socklen_t)addrlen) < 0) {
            perror("sendto\n");
            exit(1);
       }
       
       //Guardo la direccion del que me contesto primero. Es mi siguiente.
       strcpy(ipBrokerSiguiente,inet_ntoa(addr.sin_addr));
       set_siguiente(ipBrokerSiguiente);
       sprintf(mostrar,"[MASTER] --> Ip del Broker Siguiente: %s\n",ipBrokerSiguiente);
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       //Espero hasta que alguno me diga de cerrar el anillo
       
       sprintf(mostrar,"[ESPERANDO] --> Al cierre del anillo\n");
       write(fileno(stdout),mostrar,strlen(mostrar));
       do{
            if((nbytes = recvfrom(fd,&msgInvitacion,sizeof(MsgInvitacion_t),0,(struct sockaddr *)&addr,(socklen_t *)&addrlen)) < 0){
                perror("recvfrom\n");
                exit(1);
            }
       }while(msgInvitacion.tipo!=CERRAR);

       sprintf(mostrar,"[RECIBIDO] <-- %s\t%s\n",inet_ntoa(addr.sin_addr),imprimirCodigo(msgInvitacion.tipo));
       write(fileno(stdout),mostrar,strlen(mostrar));
       //Me llegó mensaje para cerrar el anillo
       //Me conecto al siguiente
       sleep(5);
       fdSiguiente = tcpopact(ipBrokerSiguiente,portTcp);
       if(fdSiguiente < 0){
           perror("tcpopact");
           exit(1);
       }
       sprintf(mostrar,"[CONEXION ESTABLECIDA] --> Broker Siguiente%s\n",ipBrokerSiguiente);
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       //Me quedo esperando hasta que el último se conecte a mi
       fdAnterior = tcpoppas(portTcp);
       if(fdAnterior < 0){
            perror("tcpoppas");
            exit(1);
        }
       
       //Acepto la conexion entrante
       addrlen = sizeof(addr);
       newsockfd = accept(fdAnterior, (struct sockaddr *) &addr, &addrlen);
       if(newsockfd < 0){
           perror("accept");
           exit(1);
       }
       
       sprintf(mostrar,"[CONEXION ESTABLECIDA] --> Broker Anterior %s\n",inet_ntoa(addr.sin_addr));
       write(fileno(stdout),mostrar,strlen(mostrar));
       
       
       //Comienza el algoritmo para elegir al lider
       //El master pone un mensaje en el anillo con su id y estado DESCONOCIDO (no se sabe quien es el lider)
       msgLider.idBroker = *id;
       msgLider.estado = DESCONOCIDO;
       
       sleep(7);
       //El master envia el mensaje al siguiente
       if( (nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
            sprintf(mostrar,"Error al enviar msgLider\n");
            write(fileno(stdout),mostrar,strlen(mostrar));
            exit(1);
        }
        
        sprintf(mostrar,"[ENVIADO] --> %s\t%s\n",ipBrokerSiguiente,imprimirCodigo(msgLider.estado));
	write(fileno(stdout),mostrar,strlen(mostrar));
       
       do{
	 //Se queda esperando a que le llegue un mensaje del broker anterior a él
           sprintf(mostrar,"[ESPERANDO] --> Recibir mensaje de: %s\n",inet_ntoa(addr.sin_addr));
	    write(fileno(stdout),mostrar,strlen(mostrar));
           nbytes = recibir(newsockfd,&msgLider,sizeof(msgLider));
           if(nbytes < 0){
               sprintf(mostrar,"Error al recibir msgLider\n");
               write(fileno(stdout),mostrar,strlen(mostrar));
               exit(1);
           }else{
               //sprintf(mostrar,"Recibido --> ID: %d Estado: %s\n",msgLider.idBroker,imprimirCodigo(msgLider.estado));
               //write(fileno(stdout),mostrar,strlen(mostrar));
               sprintf(mostrar, "[RECIBIDO] <-- ID: %d\t%s\n",msgLider.idBroker,imprimirCodigo(msgLider.estado));
	       write(fileno(stdout), mostrar, strlen(mostrar));
	       //Si el id que le llego es mayor que el del actual entonces se lo envia a su siguiente
               if((msgLider.idBroker > *id) && (msgLider.estado!=FIN)){
		   msgLider.estado=DESCONOCIDO;
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(msgLider)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
               }else if(msgLider.idBroker == *id){
		 //Si el id es igual al actual, entonces es el lider. Envia un mensaje indicando que es el fin de la busqueda
                   msgLider.estado = FIN;
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(MsgLider_t)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
                   //Se encontró al lider, avisa al siguiente y sale del while
               }else if(msgLider.estado == FIN){
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(MsgLider_t)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
               }
               else if(msgLider.idBroker < *id) {
		   msgLider.idBroker=*id;
		   msgLider.estado=DESCONOCIDO;
                   if((nbytes=enviar(fdSiguiente,&msgLider,sizeof(msgLider))) != sizeof(MsgLider_t)){
                        sprintf(mostrar,"Error al enviar msgLider\n");
                        write(fileno(stdout),mostrar,strlen(mostrar));
                        exit(1);
                   }
		   
	       }
           }
       }while((msgLider.estado != LIDER) && (msgLider.estado!=FIN));
       
       sprintf(mostrar, "[MASTER] --> ID LIDER ENCONTRADO: %d\t%s\n", msgLider.idBroker, imprimirCodigo(msgLider.estado));
       set_lider(msgLider.idBroker == *id);
       write(fileno(stdout), mostrar, strlen(mostrar));
       
       sprintf(mostrar, "::::: QUEDA ESTABLECIDO EL ANILLO :::::\n", msgLider.idBroker, imprimirCodigo(msgLider.estado));
       write(fileno(stdout), mostrar, strlen(mostrar));
	
		key = ftok("/tmp/buchwaldipcs",SEM_ANILLO_FORMANDO);
		int semid = semget(key,1, IPC_CREAT| 0660);
		struct sembuf oper;
		oper.sem_num = 0;
		oper.sem_op = 1;
		oper.sem_flg = 0;
		semop(semid,&oper,1);
		
		shmdt((void*)id);
		close(fdSiguiente);
		close(fdAnterior);
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
