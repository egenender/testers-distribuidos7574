#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string>
#include <string.h>
#include <sstream>

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
	
	//Por ahora lo hacemos mas user friendly, despues le metemos para hacerlo por consola
	
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
	
	printf("\nIngrese ip del equipo (si no va a ser broker, se puede escribir cualquiera)\n");
	char* ip_equipo = leer_linea();
	
	printf("\nIngrese ip del servidor RPC\n");
	char* ip_rpc = leer_linea();
	
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
	}
	sprintf(sed, "sed 's/broker_id_ip_t BROKERS\\[\\] = {.*};/broker_id_ip_t BROKERS[] = {%s};/' broker/ids_brokers.h > broker/ids_brokers.h.tmp", ss.str().c_str());
	system(sed);
	sprintf(sed, "sed 's/#define CANT_BROKERS .*/#define CANT_BROKERS %d/' broker/ids_brokers.h.tmp > broker/ids_brokers.h", cant_brokers);
	system(sed);
	system("rm -f broker/ids_brokers.h.tmp");
	
	/* Configuro el id del broker (broker/Broker.cpp) */ 
	printf("Configurando id broker\n");
	system("cp broker/Broker.cpp broker/Broker.cpp.tmp");
	sprintf(sed, "sed 's/#define ID_BROKER .*/#define ID_BROKER %d/' broker/Broker.cpp.tmp > broker/Broker.cpp", id_equipo);
	system(sed);
	system("rm -f broker/Broker.cpp.tmp");
	
	/* Configuro el common/common.h (rpc server)*/
	printf("Configurando ip servidor rpc\n");
	system("cp common/common.h common/common.h.tmp");
	sprintf(sed, "sed 's/const char UBICACION_SERVER_RPC\\[\\].*/const char UBICACION_SERVER_RPC[] = \"%s\";/' common/common.h.tmp > common/common.h", ip_rpc);
	system(sed);
	system("rm -f common/common.h.tmp");
	
	return 0;
}
