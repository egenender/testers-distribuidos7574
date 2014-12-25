#include "registracion.h"
#define SOLO_C
#include "../../common/common.h"
#include <sys/msg.h>

int devolver_id(char *host, int tipo, int id) {
	CLIENT *clnt;
	int  *result_id;
		
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
		result_id = devolver_id_dispositivo_1(&id, clnt);		
	}else{
		result_id = devolver_id_tester_1(&id, clnt);		
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
	int tipo, id;
	
	if (argc < 4) {
		printf ("usage: %s <server_host> <tipo> <id>\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	tipo = atoi(argv[2]);
	id = atoi(argv[3]);
	
	exit (devolver_id(host, tipo, id));
}
