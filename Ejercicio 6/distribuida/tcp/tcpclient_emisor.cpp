#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "../common/common.h"
#include "comunes_tcp.h"

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/buchwaldipcs"
#endif
 
int main(int argc, char *argv[]){
	if(argc != 6){
        printf("%s <host> <port> <id_dispositivo> <id_cola> <pid_receptor>\n",argv[0]);
        return -1;
    }
	size_t size = sizeof(TMessageAtendedor);
	int id = atoi(argv[3]);
	int id_cola = atoi(argv[4]);
	
	pid_t receptor = atoi(argv[5]);
	
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd < 0){
      perror("Error");
      return -2;
    }
    
    key_t key = ftok(IPCS_FILE, id_cola);
    int cola = msgget(key, 0660);
    
    key = ftok(IPCS_FILE, MSGQUEUE_DISPOSITIVO_RECEPTOR_EMISOR);
	int cola_id_tester = msgget(key, 0660| IPC_CREAT);
    
    /* FIN del setup */
    
    // Envio primer mensaje, para pasarle mi id al servidor
    TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
	buffer->mtype = 1;
	buffer->idDispositivo = id;
	enviar(buffer, fd);
	
	// Espero mensaje en la cola, proveniente del servidor, del id del server/tester.
	// Necesito ese id porque es con ese id que voy a esperar en la cola de emision
	
	int ok_read = msgrcv(cola_id_tester, buffer, size - sizeof(long), id, 0);
	if (ok_read == -1){
		exit(0);
	}
	long id_tester = buffer->tester;
	
	
    while (true) {
		//Espero mensaje de la cola
		int ok_read = msgrcv(cola, buffer, size - sizeof(long), id_tester, 0);
			
		if (ok_read == -1){
			exit(0);
		}
		
		//Si era un mensaje de finalizacion, 'mato' al receptor, y termino mi labor
		if (buffer->finalizar_conexion == 1){
			kill(receptor, SIGHUP);
			close(fd);
			free(buffer);
			exit(0);
		}
		
		enviar(buffer, fd);
	}
	close(fd);
    return 0;
}
