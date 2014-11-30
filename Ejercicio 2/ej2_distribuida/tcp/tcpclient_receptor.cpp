#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include "../common/common.h"

/**
 * Cliente que recibe datos del socket leyendo cada vez la cantidad de bytes indicada.
 */
int main(int argc, char *argv[]){
	if(argc != 4){
        printf("%s <host> <port> <id>\n",argv[0]);
        return -1;
    }
	size_t size = sizeof(TMessageAtendedor);
	int id = atoi(argv[3]);
	
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd < 0){
      perror("Error");
      return -2;
    }
    
    TMessageAtendedor message;
    message.mtype = 1;
    message.idDispositivo = id;
    size_t acumulado = 0;
    int enviado = 0;
    printf("Le digo al server que soy el dispositivo de id %d\n", id);
    char* buffer_primer_envio = (char*) &message;
    while((enviado = write(fd, buffer_primer_envio + acumulado, size - acumulado)) > 0 && acumulado < size){
		acumulado += enviado;
	}

	TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
    while (true) {
		int leido = 0;
		acumulado = 0;
		
		char* buffer_act = (char*) buffer;
		while( (leido = read(fd, buffer_act, size - acumulado)) > 0){
			acumulado += leido;
			buffer_act = buffer_act + leido;
		}
			
		//key_t key = ftok(ipcFileName.c_str(), buffer->cola_a_usar);
		key_t key = ftok("ipcs-prueba", buffer->cola_a_usar);
		int cola = msgget(key, 0660);
		
		int ok = msgsnd(cola, buffer, size - sizeof(long), 0);
		if (ok == -1){
			exit(0);
		}
	}

    return 0;
}
