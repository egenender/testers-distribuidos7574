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
      
    /* FIN del setup */
    
    // Envio primer mensaje, para pasarle mi id al servidor
    TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
	buffer->mtype = 1;
	buffer->idDispositivo = id;
	enviar(buffer, fd);
	
    while (true) {
		//Espero mensaje de la cola
		int ok_read = msgrcv(cola, buffer, size - sizeof(long), id, 0);			
		if (ok_read == -1){
			kill(receptor, SIGHUP);
			close(fd);
			free(buffer);
			exit(0);
		}
		
		buffer->mtype = buffer->mtype_envio;
		enviar(buffer, fd);
		
		//Si era un mensaje de finalizacion, 'mato' al receptor, y termino mi labor
		if (buffer->finalizar_conexion){
			kill(receptor, SIGHUP);
			close(fd);
			free(buffer);
			exit(0);
		}
	}
	close(fd);
    return 0;
}
