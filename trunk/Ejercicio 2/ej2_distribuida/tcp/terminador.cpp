#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "../common/common.h"

int main(void){
	key_t key = ftok("ipcs-prueba", 1);
	int cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", 2);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", 3);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", 4);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", MSGQUEUE_DISPOSITIVO_RECEPTOR_EMISOR);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", MSGQUEUE_SERVER_RECEPTOR_EMISOR);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	return 0;
}
