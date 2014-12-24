#include "registracion.h"

const int MAX_TESTERS_COMUNES = 5;
const int MAX_TESTERS_ESPECIALES = 5;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;
const int SEM_CANT_TESTERS_COMUNES = 21;
const int SEM_TABLA_TESTERS = 22;
const int SHM_TABLA_TESTERS = 23;
const int SEM_ESPECIAL_DISPONIBLE = 24;


void
register_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	char *get_id_dispositivo_1_arg;
	int  *result_2;
	int  get_id_tester_1_arg;
	int  *result_3;
	int  registrar_tester_activo_1_arg;
	int  *result_4;
	int  devolver_id_dispositivo_1_arg;
	int  *result_5;
	int  devolver_id_tester_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, REGISTER, REGISTER_V1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */
	int i;
	for (i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA - 5; i++){
		result_1 = devolver_id_dispositivo_1(&i, clnt);
		if (result_1 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
		printf ("Resultado de devolver id %d: %d\n",i, *result_1);
	}
	result_1 = get_id_dispositivo_1((void*)&get_id_dispositivo_1_arg, clnt);
	printf("Volvi a obtener un id: %d\n", *result_1);
	
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	register_1 (host);
exit (0);
}
