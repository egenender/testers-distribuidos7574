#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

int main(void){
	key_t key = ftok("ipcs-prueba", 1);
	int cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
	
	key = ftok("ipcs-prueba", 2);
	cola = msgget(key, 0660);
	msgctl(cola ,IPC_RMID, NULL);
}
