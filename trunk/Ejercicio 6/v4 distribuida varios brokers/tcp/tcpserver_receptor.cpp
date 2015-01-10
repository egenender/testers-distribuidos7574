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
	
    if(argc != 5){
		printf("Uso: %s <puerto> <id_server> <id_cola> <id_cola_emisor>\n", argv[0]);
		return -1;
    }
	int id_tester = atoi(argv[2]);
	int id_cola = atoi(argv[3]);
	int id_cola_emisor = atoi(argv[4]);
	
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
		//Verifico no estar atendiendo ya muchos clientes
		while (cant_atendidos >= MAXIMOS_ATENDIDOS){
			wait(NULL);
			cant_atendidos--;
		}		
		
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
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
			
			if (buffer->idDispositivo <= MAX_DISPOSITIVOS_EN_SISTEMA){
				int ok = msgsnd(cola_id_disp, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
				if (ok == -1){
					exit(1);
				}
			}					
			key = ftok(IPCS_FILE, id_cola);
			int cola = msgget(key, 0660);
			key = ftok(IPCS_FILE, id_cola_emisor);
			int cola_emisor = msgget(key, 0660);
			while (true) {
				//Espero por un mensaje desde el cliente
				recibir(buffer, clientfd);
				
				int cola_envio = cola;
				if(buffer->finalizar_conexion){
					cola_envio = cola_emisor;
				}
				//Mando el mensaje por la cola que el cliente me dice que tengo que usar
				int ok = msgsnd(cola_envio, buffer, sizeof(TMessageAtendedor) - sizeof(long), 0);
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
