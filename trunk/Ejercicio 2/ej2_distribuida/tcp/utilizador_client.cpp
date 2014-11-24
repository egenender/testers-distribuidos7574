#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "../common/common.h"

int main(void){
	
	key_t key = ftok("ipcs-prueba", 2);
	int cola_client = msgget(key, 0660 | IPC_CREAT);

	
	if (fork() == 0){
		execlp("./tcpclient", "tcpclient", "localhost","9000","3",(char*)0);
		exit(1);
	}
	
	TMessageAtendedor msg;
	
	int ok_read = msgrcv(cola_client, &msg, sizeof(TMessageAtendedor) - sizeof(long), 3, 0);
    if (ok_read == -1){
		printf("Error en la cola");
		exit(0);
	}
	
	printf("Se recibe mensaje desde tester %d\n", msg.idDispositivo);
}
