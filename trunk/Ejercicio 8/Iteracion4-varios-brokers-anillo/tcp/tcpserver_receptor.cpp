#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"

int main(int argc, char *argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    if(argc != 4){
        Logger::error("Bad arguments!", __FILE__);
        printf("Uso: %s <puerto> <idMsgQueue> <sizeMsg>\n", argv[0]);
        return -1;
    }

    int idMsgQueue = atoi(argv[2]);
    int sizeMsg = atoi(argv[3]);
	
    int fd = tcpOpenPasivo(atoi(argv[1]));
    if(fd < 0) {
        Logger::error("Could not create socket", __FILE__);
        return -3;
    }

    while(true) {

        int clientFd = accept(fd, (struct sockaddr*)NULL, NULL);

        if (fork() == 0) {
            void* buffer = malloc(sizeMsg);

            key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
            int msgQueue = msgget(key, 0660);
            while (true) {
                recibir(clientFd, buffer, sizeMsg);

                int ok = msgsnd(msgQueue, buffer, sizeMsg - sizeof(long), 0);
                if (ok == -1) {
                    Logger::error("Error enviando a la cola de mensajes", __FILE__);
                    close(clientFd);
                    exit(1);
                }
            }
            free(buffer);
            close(clientFd);
            exit(0);
        }
        close(clientFd);
	}
	return 0;
}
