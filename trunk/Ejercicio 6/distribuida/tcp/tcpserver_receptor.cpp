#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "comunes_tcp.h"

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/buchwaldipcs"
#endif

void terminar_ejecucion(int sig){
	// ACA HAY QUE TOCAR SI DEBERIA HACER ALGO DISTINTO A SIMPLEMENTE MORIR
	exit(0);
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
    signal(SIGHUP, terminar_ejecucion);
    /* FIN del setup */
    
    int cant_atendidos = 0;
    
    while(true){
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
		//Verifico no estar atendiendo ya muchos clientes
		while (cant_atendidos >= MAXIMOS_ATENDIDOS){
			wait(NULL);
			cant_atendidos--;
		}
		cant_atendidos++;
		
		if (fork() == 0){
			TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(sizeof(TMessageAtendedor));
			
			//Espero primer mensaje, que el cliente me tiene que decir su ID.
			recibir(buffer, clientfd);
						
			key_t key = ftok(IPCS_FILE, MSGQUEUE_SERVER_RECEPTOR_EMISOR);
			int cola_id_disp = msgget(key, 0660| IPC_CREAT);
			//Mando Primer mensaje, que me dice el identificador del cliente (al emisor) 
			//y tambien mi pid para que el emisor me 'mate' cuando todo se termine
			buffer->mtype = id_tester;
			buffer->value = getpid();
			
			int ok = msgsnd(cola_id_disp, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if (ok == -1){
				exit(1);
			}
								
			while (true) {
				//Espero por un mensaje desde el cliente
				recibir(buffer, clientfd);
				
				//Mando el mensaje por la cola que el cliente me dice que tengo que usar
				key_t key = ftok(IPCS_FILE, buffer->cola_a_usar);
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
