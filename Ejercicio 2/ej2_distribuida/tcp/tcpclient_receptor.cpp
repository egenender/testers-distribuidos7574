#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include "../common/common.h"

/**
 * Cliente que recibe datos del socket leyendo cada vez la cantidad de bytes indicada.
 */
 
void recibir(TMessageAtendedor* buffer, int fd){
	size_t size = sizeof(TMessageAtendedor);
	size_t acumulado = 0;
	int leido;
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
	if(argc != 4){
        printf("%s <host> <port> <id_dispositivo>\n",argv[0]);
        return -1;
    }
	size_t size = sizeof(TMessageAtendedor);
	
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd < 0){
		perror("Error");
		return -2;
    }
    
    long id_dispositivo = atol(argv[3]);

	TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
	recibir(buffer, fd);
	key_t key = ftok("ipcs-prueba", MSGQUEUE_DISPOSITIVO_RECEPTOR_EMISOR);
	int cola_emisor = msgget(key, 0660);
	buffer->mtype = id_dispositivo;
		
	int ok = msgsnd(cola_emisor, buffer, size - sizeof(long), 0);
	if (ok == -1){
		exit(1);
	}
	
    while (true) {
		recibir(buffer, fd);
		//key_t key = ftok(ipcFileName.c_str(), buffer->cola_a_usar);
		key_t key = ftok("ipcs-prueba", buffer->cola_a_usar);
		int cola = msgget(key, 0660| IPC_CREAT);
		
		int ok = msgsnd(cola, buffer, size - sizeof(long), 0);
		if (ok == -1){
			exit(1);
		}
	}
	close(fd);
    return 0;
}
