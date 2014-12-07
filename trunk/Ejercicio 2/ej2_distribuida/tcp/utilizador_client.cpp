#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "../common/common.h"

int main(void){
	/* BEGIN SETUP */
	key_t key = ftok("ipcs-prueba", 2);
	int cola_client_rec = msgget(key, 0660 | IPC_CREAT);
	key = ftok("ipcs-prueba", 4);
	int cola_client_em = msgget(key, 0660 | IPC_CREAT);

	pid_t receptor = fork();
	if (receptor == 0){
		execlp("./tcpclient_receptor", "tcpclient_receptor", "localhost","9000","3",(char*)0);
		exit(1);
	}
	
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	
	pid_t emisor = fork();	
	if (emisor == 0){
		execlp("./tcpclient_emisor", "tcpclient_emisor", "localhost","9001","3", "4", param_pid, (char*)0);
		exit(1);
	}
	/* FIN SETUP */
	TMessageAtendedor msg;
	
		
	int ok_read = msgrcv(cola_client_rec, &msg, sizeof(TMessageAtendedor) - sizeof(long), 3, 0);
    if (ok_read == -1){
		perror("Error en la cola");
		exit(1);
	}

	printf("Se recibe mensaje desde tester %d\n", msg.idDispositivo);
	
	msg.finalizar_conexion = 0;
	long tester = msg.idDispositivo;
	msg.mtype = tester;
	msg.idDispositivo = 3;
	msg.value = 10;
	msg.cola_a_usar = 3;
	int ok_send = msgsnd(cola_client_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
	if (ok_send == -1){
		perror("Error en la cola");
		exit(1);
	}
	
	msg.finalizar_conexion = 1;
	ok_send = msgsnd(cola_client_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
	if (ok_send == -1){
		perror("Error en la cola");
		exit(1);
	}
	
	return 0;
}
