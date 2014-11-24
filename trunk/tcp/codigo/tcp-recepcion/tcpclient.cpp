#include "../tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include "../measure.h"

/**
 * Cliente que recibe datos del socket leyendo cada vez la cantidad de bytes indicada.
 */
int main(int argc, char *argv[])
{
        if(argc != 5){
        printf("%s <host> <port> <id_cola> <tam mensaje>\n",argv[0]);
        return -1;
    }

    int size = atoi(argv[4]);
    int id_cola = atoi(argv[3]);
        
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd < 0){
      perror("Error");
      return -2;
    }
    
    tic();
    int leido = 0;
    size_t acumulado = 0;
    void* buffer = malloc(size);
    /*while( (leido = read(fd, buffer, size)) > 0){
        lecturas++;
        acumulado += leido;
    }*/
    char* buffer_act = (char*) buffer;
    while( (leido = read(fd, buffer_act, size - acumulado)) > 0){
        acumulado += leido;
        buffer_act = buffer_act + leido;
    }
    
    double time_ms=toc();

    printf("Tiempo total: %f ms.\n", time_ms);

	key_t key = ftok("ipcs-prueba", id_cola);
    int cola = msgget(key, 0660);
    int ok = msgsnd(cola, buffer, size - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}	

    return 0;
}
