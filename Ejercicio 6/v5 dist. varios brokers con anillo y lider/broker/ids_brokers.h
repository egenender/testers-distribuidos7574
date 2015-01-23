
typedef struct broker_id_ip{
	int id;
	char ip[16];
}broker_id_ip_t;

broker_id_ip_t BROKERS[] = { 
	{1001, "192.168.1.104"},
	{1002, "192.168.1.101"},
	{1003, "192.168.1.108"}
};

#define CANT_BROKERS 3
