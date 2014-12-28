#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "../common/common.h"

int main(void){
	/* Begin setup */
	key_t key = ftok("ipcs-prueba", 1);
	int cola_server_em = msgget(key, 0660 | IPC_CREAT);
	key = ftok("ipcs-prueba", 3);
	int cola_server_rec = msgget(key, 0660 | IPC_CREAT);

	pid_t emisor = fork();
	if (emisor == 0){
		execlp("./tcpserver_emisor", "tcpserver_emisor", "9000","1", "6",(char*)0);
		exit(1);
	}
	
	pid_t receptor = fork();
	if (receptor == 0){
		execlp("./tcpserver_receptor", "tcpserver_receptor", "9001","6",(char*)0);
		exit(1);
	}
	/* FIN SETUP */
	TMessageAtendedor msg;
	
	msg.mtype = 3;
	msg.tester = 6;
	msg.cola_a_usar = 2;
	msg.finalizar_conexion = 0;
	printf ("Voy a mandar mensaje a dispositivo 3\n");
	int ok = msgsnd(cola_server_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
	
	int ok_read = msgrcv(cola_server_rec, &msg, sizeof(TMessageAtendedor) - sizeof(long), 6, 0);
    if (ok_read == -1){
		perror("Error en la cola");
		exit(1);
	}	
	
	if (msg.value == 10){
		msg.mtype = 3;
		msg.tester = 6;
		msg.finalizar_conexion = 1;
		printf ("Mando mensaje de finalizacion a dispositivo 3\n");
		ok = msgsnd(cola_server_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if (ok == -1){
			exit(0);
		}	
	}
	return 0;
}
