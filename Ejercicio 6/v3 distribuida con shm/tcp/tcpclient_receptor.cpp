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
 
void terminar_ejecucion(int sig){
	// ACA HAY QUE TOCAR SI DEBERIA HACER ALGO DISTINTO A SIMPLEMENTE MORIR
	exit(0);
}
 
int main(int argc, char *argv[]){
	if(argc != 4){
        printf("%s <host> <port> <id_cola>\n",argv[0]);
        return -1;
    }
	size_t size = sizeof(TMessageAtendedor);
	
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd < 0){
		perror("Error");
		return -2;
    }
    
    int id_cola = atoi(argv[3]);
    key_t key = ftok(IPCS_FILE, id_cola);
	int cola = msgget(key, 0660| IPC_CREAT);
    
	signal(SIGHUP, terminar_ejecucion);
		
	/* FIN del setup */
	
	TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
		
	//Ciclo general
    while (true) {
		//Espero un mensaje desde el servidor
		recibir(buffer, fd);
		//Mando el mensaje por la cola que me indique el server		
		int ok = msgsnd(cola, buffer, size - sizeof(long), 0);
		if (ok == -1){
			exit(1);
		}
	}
	close(fd);
    return 0;
}
