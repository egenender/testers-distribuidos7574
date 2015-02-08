#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "../common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"
/*
#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/pereira-ipcs"
#endif

void terminar_ejecucion(int sig){
	// ACA HAY QUE TOCAR SI DEBERIA HACER ALGO DISTINTO A SIMPLEMENTE MORIR
	exit(0);
}
*/
int main(int argc, char *argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    if(argc != 5) {
        Logger::error("Bad arguments!", __FILE__);
        printf("%s <host> <port> <id> <idMsgQueue>\n",argv[0]);
        return -1;
    }
    size_t size = sizeof(TMessageAtendedor);
	
    int fd = tcpOpenActivo(argv[1], atoi(argv[2]));
    if(fd < 0) {
        Logger::error("Error al crear el socket", __FILE__);
        return -2;
    }
    
    int id = atoi(argv[3]);
    int idMsgQueue = atoi(argv[4]);
    key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
    int msgQueue = msgget(key, IPC_CREAT | 0666);

    //signal(SIGHUP, terminar_ejecucion);

    /* FIN del setup */
    
    // Le envio el identificador correspondiente al server emisor
    // Para que sepa IDs de quien buscar mensajes
    TFirstMessage* firstMsg = (TFirstMessage*) malloc (sizeof(TFirstMessage));
    firstMsg->identificador = id;
    enviar(fd, firstMsg, sizeof(TFirstMessage));

    //Ciclo general
    TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
    while (true) {
        //Espero un mensaje desde el servidor
        recibir(fd, buffer, size);
        if (buffer->finalizar_conexion) {
            exit(0);
        } else {
            //Mando el mensaje por la cola que me indique el server
            int ok = msgsnd(msgQueue, buffer, size - sizeof(long), 0);
            if (ok == -1) {
                exit(1);
            }
        }
    }
    close(fd);
    return 0;
}
