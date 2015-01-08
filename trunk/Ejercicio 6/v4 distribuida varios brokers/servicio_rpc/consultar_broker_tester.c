#include "registracion.h"
#define SOLO_C
#include "../common/common.h"
#include <sys/msg.h>

int consultar_broker(char *host, int id_tester) {
	CLIENT *clnt;
	int  *result_id;
	char *get_id_dispositivo_1_arg;
	
#ifndef	DEBUG
	clnt = clnt_create (host, REGISTER, REGISTER_V1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	
	
	result_id = broker_de_tester_1 (&id_tester, clnt);
	
	if (result_id == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
			
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
	return *result_id;
}


int main (int argc, char *argv[]) {
	char *host;
	int id_tester;
	
	if (argc < 3) {
		printf ("usage: %s <server_host> <id_tester>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	id_tester = atoi(argv[2]);
	
	TMessageAtendedor msg;
	msg.mtype = MTYPE_CONSULTA_BROKER;
	msg.value = consultar_broker(host, id_tester);
	
	key_t key = ftok("/tmp/buchwaldipcs", MSGQUEUE_GETTING_IDS);
    int cola_ids = msgget(key, 0666 | IPC_CREAT);
	
	int ret = msgsnd(cola_ids, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        exit(1);
    }
	
	return 0;
}
