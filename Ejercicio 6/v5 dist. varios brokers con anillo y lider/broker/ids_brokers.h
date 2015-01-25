#include <stdlib.h>

typedef struct broker_id_ip{
	int id;
	char ip[16];
}broker_id_ip_t;

broker_id_ip_t BROKERS[] = {{201, "192.168.1.101"}};

#define CANT_BROKERS 1

char* obtener_broker_random(){
	int random_index = rand() % CANT_BROKERS;
	return BROKERS[random_index].ip;
}
