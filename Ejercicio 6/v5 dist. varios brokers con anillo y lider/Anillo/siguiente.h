#include "../broker/ids_brokers.h"
#define SOLO_C
#include "../common/common.h"
#include <sys/shm.h>

void set_siguiente(char* ip){
	key_t key = ftok("/tmp/buchwaldipcs", SHM_NEXT_BROKER);
	int shmnext = shmget(key, sizeof(int) , 0660);
	int* next = (int*)shmat(shmnext, NULL, 0);   

	*next = 0;
	int i;
	for (i = 0; i < sizeof(BROKERS) && *next == 0; i++){
		if (strcmp(ip, BROKERS[i].ip) == 0){
			*next = BROKERS[i].id;
		}
	}			
	shmdt(next);
}

void set_lider(int es_lider){
	key_t key = ftok("/tmp/buchwaldipcs", SHM_LIDER);
	int shmlider = shmget(key, sizeof(int) , 0660);
	int* lider = (int*)shmat(shmlider, NULL, 0);   
	*lider = es_lider;
	shmdt(lider);
}


