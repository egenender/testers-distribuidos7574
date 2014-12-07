#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include "../common/common.h"

void recibir (TMessageAtendedor* buffer, int fd){
	size_t size = sizeof(TMessageAtendedor);
	size_t acumulado = 0;
	int leido = 0;
	char* buffer_act = (char*) buffer;
	while( (leido = read(fd, buffer_act, size - acumulado)) >= 0 && acumulado < size){
		acumulado += leido;
		buffer_act = buffer_act + leido;
	}
	if (acumulado != size){
		perror("Error recibiendo mensaje desde el servidor");
		exit(1);
	}
}

int main(int argc, char *argv[]){
	
    if(argc != 3){
		printf("Uso: %s <puerto> <id_tester>\n", argv[0]);
		return -1;
    }
	int id_tester = atoi(argv[2]);

    int fd = tcp_open_pasivo(atoi(argv[1]));
    if(fd < 0){
      perror("Error");
      return -3;
    }
    
    tcp_disable_nagle(fd);

    if(listen(fd, 10) != 0){
      perror("Error en el listen");
      return -4;
    }

    signal(SIGPIPE, SIG_IGN);
    
    while(1){
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
		
		if (fork() == 0){
			TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(sizeof(TMessageAtendedor));
			
			recibir(buffer, clientfd);
						
			key_t key = ftok("ipcs-prueba", MSGQUEUE_SERVER_RECEPTOR_EMISOR);
			int cola_id_disp = msgget(key, 0660| IPC_CREAT);
			//Mando Primer mensaje, que me dice el identificador del cliente (al emisor)   
			buffer->mtype = id_tester;
			buffer->value = getpid();
			int ok = msgsnd(cola_id_disp, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if (ok == -1){
				exit(1);
			}
								
			while (true) {
				recibir(buffer, clientfd);
				
				//key_t key = ftok(ipcFileName.c_str(), id_cola);
				key_t key = ftok("ipcs-prueba", buffer->cola_a_usar);
				int cola = msgget(key, 0660);
				int ok = msgsnd(cola, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
				if (ok == -1){
					exit(1);
				}
			}
			free(buffer);
			close(clientfd);
			exit(0);
			
		}
		close(clientfd);
	}
	
}
