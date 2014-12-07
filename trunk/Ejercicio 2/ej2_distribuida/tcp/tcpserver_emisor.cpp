#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include "../common/common.h"

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/buchwaldipcs"
#endif

void enviar(TMessageAtendedor* buffer, int fd){
	size_t size = sizeof(TMessageAtendedor);
	size_t acumulado = 0;
	int enviado = 0;
	char* buffer_envio = (char*) buffer;			
	while((enviado = write(fd, buffer_envio + acumulado, size - acumulado)) >= 0 && acumulado < size){
		acumulado += enviado;
	}
	if (acumulado != size){
		perror("Error al enviar el mensaje al cliente");
	}
}

int main(int argc, char *argv[]){
	
    if(argc != 4){
		printf("Uso: %s <puerto> <id_cola> <id_tester>\n", argv[0]);
		return -1;
    }
	
	int id_cola = atoi(argv[2]);
	long id_tester = atol(argv[3]);
	
    size_t size = sizeof(TMessageAtendedor);
    
    if(size <= 0){
      printf("Error: el tamaño del mensaje debe ser mayor a cero\n");
      return -2;
    }

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
    
    key_t key = ftok(IPCS_FILE, id_cola);
    int cola = msgget(key, 0660);
		
    while(1){
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
		
		if (fork() == 0){
			TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
			key_t key = ftok(IPCS_FILE, MSGQUEUE_SERVER_RECEPTOR_EMISOR);
			int cola_id_disp = msgget(key, 0660| IPC_CREAT);
			//Espero Primer mensaje, que me dice el identificador del cliente    
			int ok_read = msgrcv(cola_id_disp, buffer, size - sizeof(long), id_tester, 0);
				if (ok_read == -1){
					exit(0);
			}
					
			long dispositivo_a_tratar = buffer->idDispositivo;
			pid_t receptor = buffer->value;
			
			buffer->idDispositivo = id_tester;
			enviar(buffer, clientfd);
			
			while (true){
				int ok_read = msgrcv(cola, buffer, size - sizeof(long), dispositivo_a_tratar, 0);
				if (ok_read == -1){
					exit(1);
				}
				if (buffer->finalizar_conexion){
					kill(receptor, SIGHUP);
					exit(0);
				}				
				enviar(buffer, clientfd);
			}
			free(buffer);
			close(clientfd);
			exit(0);
			
		}
		close(clientfd);
	}
	
}
