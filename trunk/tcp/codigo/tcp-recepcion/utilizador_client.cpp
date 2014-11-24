#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string>

#define TAM_BUFFER 10

typedef struct mensaje{
	long mtype;
	char buffer[TAM_BUFFER + 1];
} mensaje_t;

int main(void){

	key_t key = ftok("ipcs-prueba", 2);
	int cola_client = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
		
	if (fork() == 0){
		execlp("./tcpclient", "tcpclient", "localhost","9000","2","16" , (char*)0);
		exit(1);
	}
	
	mensaje_t otro_mensaje;
	
	int ok_read = msgrcv(cola_client, &otro_mensaje, sizeof(mensaje_t) - sizeof(long), 1, 0);
    if (ok_read == -1){
		printf("Error en la cola");
		exit(0);
	}
	
	printf("El mensaje recibido fue %s\n", otro_mensaje.buffer);
}
