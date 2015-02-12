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

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/pereira-ipcs"
#endif
/*
void terminar_ejecucion(int sig){
    // ACA HAY QUE TOCAR SI DEBERIA HACER ALGO DISTINTO A SIMPLEMENTE MORIR
    exit(0);
}
*/
int main(int argc, char *argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    if(argc != 3){
        //printf("Uso: %s <puerto> <id_server> <id_cola>\n", argv[0]);
        Logger::error("Bad arguments!", __FILE__);
        printf("Uso: %s <puerto> <idMsgQueue>\n", argv[0]);
        return -1;
    }

    int idMsgQueue = atoi(argv[2]);
	
    int fd = tcpOpenPasivo(atoi(argv[1]));
    if(fd < 0) {
        Logger::error("Could not create socket", __FILE__);
        return -3;
    }
/*
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, terminar_ejecucion);
 */
    // FIN del setup
    
    //int cant_atendidos = 0;
    
    while(true) {
        //Verifico no estar atendiendo ya muchos clientes
        /*while (cant_atendidos >= MAXIMOS_ATENDIDOS) {
            wait(NULL);
            cant_atendidos--;
        }		
        */
        int clientFd = accept(fd, (struct sockaddr*)NULL, NULL);
        //cant_atendidos++;

        if (fork() == 0) {
            TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(sizeof(TMessageAtendedor));

            //Espero primer mensaje, que el cliente me tiene que decir su ID.
            //recibir(buffer, clientfd);

            //key_t key = ftok(IPCS_FILE, MSGQUEUE_SERVER_RECEPTOR_EMISOR);
            //int cola_id_disp = msgget(key, 0660| IPC_CREAT);
            //Mando Primer mensaje, que me dice el identificador del cliente (al emisor) 
            //y tambien mi pid para que el emisor me 'mate' cuando todo se termine
            //buffer->mtype = id_tester;
            //buffer->value = getpid();
/*
            int ok = msgsnd(cola_id_disp, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if (ok == -1){
                    exit(1);
            }
*/
            key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
            int msgQueue = msgget(key, 0660);
            while (true) {
                //Espero por un mensaje desde el cliente
                recibir(clientFd, buffer, sizeof(TMessageAtendedor));

                //Mando el mensaje por la cola que el cliente me dice que tengo que usar
                int ok = msgsnd(msgQueue, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
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
