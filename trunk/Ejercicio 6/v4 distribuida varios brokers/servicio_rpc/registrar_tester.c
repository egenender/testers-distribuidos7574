#include "registracion.h"
#define SOLO_C
#include "../common/common.h"
#include <sys/msg.h>

int registrar_tester(char *host, int id_broker, int id_tester) {
	CLIENT *clnt;
	int  *result;
		
#ifndef	DEBUG
	clnt = clnt_create (host, REGISTER, REGISTER_V1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	
	
	par_broker_tester bt;
	bt.id_broker = id_broker;
	bt.id_tester = id_tester;
	result = registrar_tester_activo_1(&bt, clnt);
		
	if (result == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
			
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
	return *result;
}


int main (int argc, char *argv[]) {
	char *host;
	int id_broker, id_tester;
	
	if (argc < 4) {
		printf ("usage: %s <server_host> <id_broker> <id_tester>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	id_broker = atoi(argv[2]);
	id_tester = atoi(argv[3]);
	
	exit (registrar_tester(host, id_broker, id_tester));
}
