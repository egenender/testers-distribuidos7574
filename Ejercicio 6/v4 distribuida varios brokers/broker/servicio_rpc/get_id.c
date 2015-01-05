#include "registracion.h"
#define SOLO_C
#include "../../common/common.h"
#include <sys/msg.h>

int get_id(char *host, int tipo) {
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
	if (tipo < 0 || tipo > TIPO_DISPOSITIVO){
		return -1;
	}
	if (tipo == TIPO_DISPOSITIVO){
		result_id = get_id_dispositivo_1((void*)get_id_dispositivo_1_arg, clnt);		
	}else{
		result_id = get_id_tester_1(&tipo, clnt);		
	}
	
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
	int tipo;
	
	if (argc < 3) {
		printf ("usage: %s <server_host> <tipo>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	tipo = atoi(argv[2]);
	
	TMessageAtendedor msg;
	msg.mtype = tipo + 1;
	msg.value = get_id(host, tipo);
	
	key_t key = ftok("/tmp/buchwaldipcs", MSGQUEUE_GETTING_IDS);
    int cola_ids = msgget(key, 0666 | IPC_CREAT);
	
	int ret = msgsnd(cola_ids, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        exit(1);
    }
	
	return 0;
}
