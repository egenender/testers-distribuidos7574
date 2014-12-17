#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include "../common/common.h"

int main(void){
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
	int cola_desde_dispositivos = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ATENCION_REQUERIMIENTOS_DISPOSITIVOS);
	int cola_at_req = msgget(key, 0660 );
	
	/* Fin setup */
	TMessageAtendedor msg;
	
	while (true){
		int ok_read = msgrcv(cola_desde_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0); //LEO TODOS
		if (ok_read == -1){
			exit(0);
		}
		
		if (msg.es_requerimiento){
			int ret = msgsnd(cola_at_req, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
		}else {
			int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
		}		
	}
}
