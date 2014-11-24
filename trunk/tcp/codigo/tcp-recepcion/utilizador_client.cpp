#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define TAM_BUFFER 10

typedef struct mensaje{
	long mtype;
	char buffer[TAM_BUFFER + 1];
} mensaje_t;

int main(void){

	key_t key = ftok("ipcs-prueba", 2);
	int cola_client = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
	
	mensaje_t mensaje;
	printf("Tam del mensaje: %zu\n", sizeof(mensaje_t));
	
	mensaje_t otro_mensaje;
	
	int ok_read = msgrcv(cola_client, &otro_mensaje, sizeof(mensaje_t) - sizeof(long), 1, 0);
    if (ok_read == -1){
		exit(0);
	}
	
	printf("El mensaje recibido fue %s\n", otro_mensaje.buffer);
}
