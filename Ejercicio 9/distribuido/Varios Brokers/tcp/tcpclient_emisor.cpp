#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"


int main(int argc, char *argv[]){
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    if(argc != 6) {
        Logger::error("Bad arguments", __FILE__);
        printf("%s <host> <port> <identificador> <idMsgQueue> <sizeMsg>",argv[0]);
        return -1;
    }

    int id = atoi(argv[3]); // Id de Disp, o de Tester o de EqEsp, dependiendo de quien lo llame
    int idMsgQueue = atoi(argv[4]);
    int sizeMsg = atoi(argv[5]);

    int fd = tcpOpenActivo(argv[1], atoi(argv[2]));
    if(fd < 0) {
        Logger::error("Could not create socket", __FILE__);
        return -2;
    }
    
    key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
    int msgQueue = msgget(key, 0660);
    
    // Envio primer mensaje, para pasarle mi id al servidor
    void* buffer = malloc(sizeMsg);
    size_t size = sizeMsg;
    while (true) {
        int okRead = msgrcv(msgQueue, buffer, size - sizeof(long), id, 0);
        if (okRead == -1) {
            Logger::error("Error de lectura de la cola de mensajes", __FILE__);
            close(fd);
            exit(1);
        }
        enviar(fd, buffer, size);
    }
    
    close(fd);
    return 0;
}
