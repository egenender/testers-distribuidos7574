#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "../common/common.h"

int main(void){
	
	key_t key = ftok("ipcs-prueba", 1);
	int cola_server = msgget(key, 0660 | IPC_CREAT);

	
	if (fork() == 0){
		execlp("./tcpserver", "tcpserver", "9000","1",(char*)0);
		exit(1);
	}
	
	TMessageAtendedor msg;
	
	msg.mtype = 3;
	msg.idDispositivo = 7;
	msg.cola_a_usar = 2;
	
	printf ("Voy a mandar mensaje a dispositivo 3\n");
	int ok = msgsnd(cola_server, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
	return 0;
}
