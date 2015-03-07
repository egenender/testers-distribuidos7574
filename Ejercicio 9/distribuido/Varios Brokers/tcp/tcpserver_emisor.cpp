#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"

int main(int argc, char *argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    if(argc != 4) {
        printf("Uso: %s <puerto> <idMsgQueue> <sizeMsg> \n", argv[0]);
        Logger::error("Bad arguments!", __FILE__);
        return -1;
    }
	
    int idMsgQueue = atoi(argv[2]);
    size_t size = atoi(argv[3]);
   
    int fd = tcpOpenPasivo(atoi(argv[1]));
    if(fd < 0) {
        Logger::error("Error creating server socket", __FILE__);
        return -2;
    }
	
    key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
    int msgQueue = msgget(key, 0660);
	
    while(true) {
        int clientFd = accept(fd, (struct sockaddr*)NULL, NULL);

        if (fork() == 0) {
            void* buffer = malloc(size);

            //Espero Primer mensaje, que me dice el identificador del cliente
            TFirstMessage firstMsg;
            recibir(clientFd, &firstMsg, sizeof(TFirstMessage));
            int idCliente = firstMsg.identificador;
            while (true) {
                //Espero un mensaje que deba ser enviado al dispositivo en cuestion
                int okRead = msgrcv(msgQueue, buffer, size - sizeof(long), idCliente, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error recibiendo mensaje de la cola para el cliente " << idCliente <<". Errno: " << strerror(errno);
                    Logger::error(ss.str(), __FILE__);
                    close(clientFd);
                    exit(1);
                }

                enviar(clientFd, buffer, size);
            }			
        }
        close(clientFd);
    }
	
}
