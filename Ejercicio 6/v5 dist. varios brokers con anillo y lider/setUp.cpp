#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string>
#include <string.h>
#include <sstream>
#include "common/commonFiles.h"

#define MAXIMOS_BROKERS 10
#define SALIDA "0"
#define PUERTO_TCP_ANILLO 8003
#define INICIO_BROKER 201

#define TAMANIO_INICIAL_BUFFER 20
#define FIN_LINEA '\n'
#define FIN_STRING '\0'


char* leer_linea(){
	char c;	
	size_t tam = TAMANIO_INICIAL_BUFFER; 
	size_t i;

	char* buffer = (char*) malloc(sizeof(char)*(tam));
	if (!buffer) return NULL;

	for(i = 0; (c = getchar()) != FIN_LINEA && c != EOF; i++){ //leo hasta el enter o fin de archivo

		if(i + 1 == tam){ // si llego al tamanio de mi cadena
			tam += tam;  // la forma de modificar tam es arbitraria
			char* aux = (char*) realloc(buffer, tam * sizeof(char));
			if(!aux) break;  // si falla realloc, termino aca
			buffer = aux;
		}

		buffer[i] = c;
	}
	buffer[i] = FIN_STRING; // es necesario cerrar el string
	return buffer;
}


int main(void){
	/* Este setup configuran los archivos:
			- Anillo/config (ip del equipo)
			- broker/ids_brokers.h (ids e ips de brokers, asi como cantidad)
			- broker/Broker.cpp (id) 
			- common/common.h (ubicacion de servidor rpc)
	*/ 
	
	//Por ahora lo hacemos mas user friendly.
	
	printf("Ingrese las direcciones de los brokers (en orden), %s para finalizar la entrada. Maximo %d\n",SALIDA ,MAXIMOS_BROKERS);
	char* brokers[MAXIMOS_BROKERS];
	size_t cant_brokers = 0;
	
	while (cant_brokers < MAXIMOS_BROKERS){
		char* linea = leer_linea();
		if (strcmp(linea, SALIDA) == 0){
			free(linea);
			break;
		}
		//No se valida que se escriba cualquier ganzada!!
		brokers[cant_brokers++] = linea;
	}
	
	printf("\nIngrese ip del equipo\n");
	char* ip_equipo = leer_linea();
	
	printf("\nIngrese ip del servidor RPC\n");
	char* ip_rpc = leer_linea();
	
	printf("\nIngrese cantidad de testers comunes\n");
	char* tc = leer_linea();
	int cant_tc = atoi(tc);
	free(tc);
	
	printf("\nIngrese cantidad de testers especiales\n");
	char* te = leer_linea();
	int cant_te = atoi(te);
	free(te);
	
	printf("\nIngrese probabilidad (0 a 10) de tener testeos especiales\n");
	char* testing_especial = leer_linea();
	int prob_testing = atoi(testing_especial);
	free(testing_especial);
	
	printf("\nIngrese direccion default de broker\n");
	char* ip_broker_default = leer_linea();
	
	printf("\nIngrese probabilidad de fallas del broker\n");
	char* prob_falla_c = leer_linea();
	float prob_falla_f = atof(prob_falla_c);
	int prob_falla = prob_falla_f * 10;
	free(prob_falla_c);
	
	//Por si acaso, pongo la creacion del archivo de ipcs!
	printf("Creando archivo de ipcs\n");
	char touch[100];
	sprintf(touch, "touch %s", ipcFileName.c_str());
	system(touch);
	
	char sed[300];
	/* Cambio Anillo/config, ip del equipo*/ 
	printf("Configurando ip del equipo\n");
	system("cp Anillo/config Anillo/config.tmp");
	sprintf(sed, "sed 's/.* %d/%s %d/' Anillo/config.tmp > Anillo/config", PUERTO_TCP_ANILLO, ip_equipo, PUERTO_TCP_ANILLO);
    system(sed);
	system("rm -f Anillo/config.tmp");
	
	/* Cambio broker/ids_brokers.h */
	printf("Configurando brokers\n");
	std::stringstream ss;
	int id_equipo = 0;
	int num_broker = INICIO_BROKER;
	for (size_t i = 0; i < cant_brokers; i++, num_broker++){
		ss << "{" << num_broker << ", \"" << brokers[i] << "\"}";
		if (i + 1 < cant_brokers){
			ss << ", ";
		}
		if (strcmp(brokers[i], ip_equipo) == 0){
			id_equipo = num_broker;
		}
		free(brokers[i]);
	}
	sprintf(sed, "sed 's/broker_id_ip_t BROKERS\\[\\] = {.*};/broker_id_ip_t BROKERS[] = {%s};/' broker/ids_brokers.h > broker/ids_brokers.h.tmp", ss.str().c_str());
	system(sed);
	sprintf(sed, "sed 's/#define CANT_BROKERS .*/#define CANT_BROKERS %d/' broker/ids_brokers.h.tmp > broker/ids_brokers.h", cant_brokers);
	system(sed);
	system("rm -f broker/ids_brokers.h.tmp");
	
	free(ip_equipo);
	
	/* Configuro el id del broker (broker/Broker.cpp) */ 
	printf("Configurando id broker\n");
	system("cp broker/Broker.cpp broker/Broker.cpp.tmp");
	sprintf(sed, "sed 's/#define ID_BROKER .*/#define ID_BROKER %d/' broker/Broker.cpp.tmp > broker/Broker.cpp", id_equipo);
	system(sed);
	system("rm -f broker/Broker.cpp.tmp");
	
	/* Configuro el common/common.h (rpc server)*/
	printf("Configurando ip servidor rpc y demas constatnes\n");
	sprintf(sed, "sed 's/#define MAX_TESTERS_ESPECIALES .*/#define MAX_TESTERS_ESPECIALES %d/' common/common.h > common/common.h.tmp", cant_te);
	system(sed);
	sprintf(sed, "sed 's/#define MAX_TESTERS_COMUNES .*/#define MAX_TESTERS_COMUNES %d/' common/common.h.tmp > common/common.h", cant_tc);
	system(sed);
	sprintf(sed, "sed 's/const char UBICACION_BROKER\\[\\] = .*;/const char UBICACION_BROKER[] = \"%s\";/' common/common.h > common/common.h.tmp", ip_broker_default);
	system(sed);
	sprintf(sed, "sed 's/const int PROBABILIDAD_FALLA_BROKER = .*;/const int PROBABILIDAD_FALLA_BROKER = %d;/' common/common.h.tmp > common/common.h", prob_falla);
	system(sed);
	sprintf(sed, "sed 's/const char UBICACION_SERVER_RPC\\[\\].*/const char UBICACION_SERVER_RPC[] = \"%s\";/' common/common.h > common/common.h.tmp", ip_rpc);
	system(sed);
	system("cp common/common.h.tmp common/common.h");
	system("rm -f common/common.h.tmp");
	
	free(ip_broker_default);
	free(ip_rpc);
	
	/* Probabilidad de testeos */
	printf("Configurando probabilidad de testeos especiales\n");
	system("cp Dispositivo.cpp Dispositivo.cpp.tmp");
	sprintf(sed, "sed 's/if (resul >= .*)/if (resul >= %d)/' Dispositivo.cpp.tmp > Dispositivo.cpp", 10 - prob_testing);
	system(sed);
	system("rm Dispositivo.cpp.tmp");
	
	/* Compilando el logger */
	printf("Compilando el logger\n");
	system("g++ -c logger/Logger.cpp");
	system("mv Logger.o logger/Logger.o");
	
	return 0;
}
