#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "../common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"

int main(int argc, char *argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    if(argc != 6) {
        Logger::error("Bad arguments!", __FILE__);
        printf("%s <host> <port> <id> <idMsgQueue> <sizeMsg> \n",argv[0]);
        return -1;
    }
    size_t size = atoi(argv[5]);
	
    int fd = tcpOpenActivo(argv[1], atoi(argv[2]));
    if(fd < 0) {
        Logger::error("Error al crear el socket", __FILE__);
        return -2;
    }
    
    int id = atoi(argv[3]);
    int idMsgQueue = atoi(argv[4]);
    key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
    int msgQueue = msgget(key, IPC_CREAT | 0666);

    // Le envio el identificador correspondiente al server emisor
    // Para que sepa IDs de quien buscar mensajes
    TFirstMessage* firstMsg = (TFirstMessage*) malloc (sizeof(TFirstMessage));
    firstMsg->identificador = id;
    enviar(fd, firstMsg, sizeof(TFirstMessage));

    void* buffer = malloc(size);
    while (true) {
        recibir(fd, buffer, size);

        int ok = msgsnd(msgQueue, buffer, size - sizeof(long), 0);
        if (ok == -1) {
            close(fd);
            exit(1);
        }
    }
    close(fd);
    return 0;
}