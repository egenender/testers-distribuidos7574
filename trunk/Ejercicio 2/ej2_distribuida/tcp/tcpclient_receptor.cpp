#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include "../common/common.h"
#include "comunes_tcp.h"

#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/buchwaldipcs"
#endif
 
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
	key_t key = ftok(IPCS_FILE, MSGQUEUE_DISPOSITIVO_RECEPTOR_EMISOR);
	int cola_emisor = msgget(key, 0660);
	buffer->mtype = id_dispositivo;
		
	int ok = msgsnd(cola_emisor, buffer, size - sizeof(long), 0);
	if (ok == -1){
		exit(1);
	}
	
    while (true) {
		recibir(buffer, fd);
		key_t key = ftok(IPCS_FILE, buffer->cola_a_usar);
		int cola = msgget(key, 0660| IPC_CREAT);
		
		int ok = msgsnd(cola, buffer, size - sizeof(long), 0);
		if (ok == -1){
			exit(1);
		}
	}
	close(fd);
    return 0;
}
