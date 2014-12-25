#include "registracion.h"
#define SOLO_C
#include "../../common/common.h"
#include <sys/msg.h>

int registrar_id(char *host, int id) {
	CLIENT *clnt;
	int  *result;
	
#ifndef	DEBUG
	clnt = clnt_create (host, REGISTER, REGISTER_V1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	
	result = registrar_tester_activo_1(&id, clnt);
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
	int id;
	
	if (argc < 3) {
		printf ("usage: %s <server_host> <id>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	id = atoi(argv[2]);
	
	return registrar_id(host, id);
}
