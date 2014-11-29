#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#define TAM_BUFFER 10
#define ID_COLA 1
#define ID_LECTURA 1

typedef struct mensaje{
	long mtype;
	char buffer[TAM_BUFFER + 1];
} mensaje_t;


int main(void){
	srand(getpid());
	
	key_t key = ftok("ipcs-prueba", ID_COLA);
	int cola_server = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
	
	if (fork() == 0){
		execlp("./tcpserver", "tcpserver", "9000","1","16" ,"1" , (char*)0);
		exit(1);
	}
	mensaje_t mensaje;
	mensaje.mtype = ID_LECTURA;
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
	return 0;
}
