
#ifndef INET_H
#define	INET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <time.h>
#include <error.h>
#include <string.h>
#include <limits.h>
#include <errno.h>


//#include <iostream>
//#include <sstream>

//using namespace std;

//DIRECCIONES Y PUERTOS
#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
//#define PORT 23457

#define PUERTO_UDP 8003
//#define PUERTO_ANTERIOR_TCP 8004
#define PUERTO_SIGUIENTE_TCP 8005

/* DEFINO LOS TIPOS DE MENSAJES */
#define INVITACION 200
#define UNIRME 201
#define UNIDO 202
#define NO_UNIDO 203
#define CERRAR 204

//MENSAJES PARA EL ALGORITMO DE ELECCION DE LIDER
#define DESCONOCIDO 300
#define LIDER 301
#define FIN 302

//IP DEL MASTER
#define IPMASTER "192.168.25.187"

typedef struct{
    int tipo;
} MsgMulticast_t;

typedef struct{
    int tipo;
    char direccionMaster[15]; 
} MsgInvitacion_t;


typedef struct{
    int idBroker;
    int estado;
}MsgLider_t;

//#define DIRECTORIO "/home/nico/cartier/ej3-v0"
   
#define WHITE "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"

#endif
