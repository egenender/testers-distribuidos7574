#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "comunes_tcp.h"

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/buchwaldipcs"
#endif

int main(int argc, char *argv[]){
	
    if(argc != 4){
		printf("Uso: %s <puerto> <id_server> <id_cola> \n", argv[0]);
		return -1;
    }
	
	long id_tester = atol(argv[2]);
	int id_cola = atoi(argv[3]);
	
    size_t size = sizeof(TMessageAtendedor);
   
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
			TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
			
			//Espero Primer mensaje, que me dice el identificador del cliente + pid del receptor, para poder 'matarlo'
			key_t key = ftok(IPCS_FILE, MSGQUEUE_SERVER_RECEPTOR_EMISOR);
			int cola_id_disp = msgget(key, 0660| IPC_CREAT);
			
			int ok_read = msgrcv(cola_id_disp, buffer, sizeof(TMessageAtendedor) - sizeof(long), id_tester, 0);
			if (ok_read == -1){
				exit(0);
			}
					
			long dispositivo_a_tratar = buffer->idDispositivo;
			pid_t receptor = buffer->value;
			
			buffer->tester = id_tester;
			
			while (true){
				//Espero un mensaje que deba ser enviado al dispositivo en cuestion
				int ok_read = msgrcv(cola, buffer, size - sizeof(long), dispositivo_a_tratar, 0);
				
				//Si el mensaje era de finalizacion, entonces 'mato' al receptor y termino mi labor
				if (ok_read == -1 || buffer->finalizar_conexion){
					kill(receptor, SIGHUP);
					free(buffer);
					close(clientfd);
					exit(0);
				}				
				enviar(buffer, clientfd);
			}			
		}
		close(clientfd);
	}
	
}
