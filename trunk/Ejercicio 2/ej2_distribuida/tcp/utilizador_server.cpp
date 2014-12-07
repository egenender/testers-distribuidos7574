#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "../common/common.h"

int main(void){
	
	key_t key = ftok("ipcs-prueba", 1);
	int cola_server_em = msgget(key, 0660 | IPC_CREAT);
	key = ftok("ipcs-prueba", 3);
	/*int cola_server_rec = */msgget(key, 0660 | IPC_CREAT);

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
	
	TMessageAtendedor msg;
	
	msg.mtype = 3;
	msg.idDispositivo = 6;
	msg.cola_a_usar = 2;
	msg.finalizar_conexion = 0;
	printf ("Voy a mandar mensaje a dispositivo 3\n");
	int ok = msgsnd(cola_server_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
	
	msg.mtype = 3;
	msg.idDispositivo = 6;
	msg.finalizar_conexion = 1;
	printf ("Mando mensaje de finalizacion a dispositivo 3\n");
	ok = msgsnd(cola_server_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
	
	msg.mtype = 7;
	msg.idDispositivo = 3;
	msg.finalizar_conexion = 0;
	printf ("Voy a mandar mensaje a dispositivo 7\n");
	ok = msgsnd(cola_server_em, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if (ok == -1){
		exit(0);
	}
	
	
	
	return 0;
}
