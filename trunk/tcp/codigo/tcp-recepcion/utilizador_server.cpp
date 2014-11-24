#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM_BUFFER 10

typedef struct mensaje{
	long mtype;
	char buffer[TAM_BUFFER + 1];
} mensaje_t;


int main(void){
	srand(getpid());
	
	key_t key = ftok("ipcs-prueba", 1);
	int cola_server = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);

	mensaje_t mensaje;
	printf("Tam del mensaje: %zu\n", sizeof(mensaje_t));
	
	mensaje.mtype = 1;
	int i;
	for (i = 0; i < TAM_BUFFER; i++){
		mensaje.buffer[i] = 'A' + rand() % 26;
	}
	mensaje.buffer[TAM_BUFFER] = '\0';
	printf("Voy a mandar %s\n", mensaje.buffer);
	
	
	int ok = msgsnd(cola_server, &mensaje, sizeof(mensaje_t) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
}
