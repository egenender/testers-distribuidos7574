#include "registracion.h"
#include <stdbool.h>
#include <stdio.h>
#define SOLO_C
#include "../../common/common.h"

bool ids_dispositivos_disponibles[MAX_DISPOSITIVOS_EN_SISTEMA]; //ver si ampliamos los ids
int siguiente_id_dispositivo;
bool ids_tester_disponibles[MAX_TESTERS_COMUNES + MAX_TESTERS_ESPECIALES]; //ver si ampliamos los ids
int siguiente_id_tester_comun;
int siguiente_id_tester_especial;
bool inicializado = false;

tabla_testers_disponibles_t* tabla;

void inicializar(){
	int i;
	for (i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++){
		ids_dispositivos_disponibles[i] = true;
	}
	siguiente_id_dispositivo = 0; //se le sumara uno, para no tener problemas con los mtypes
	
	for (i = 0; i < MAX_TESTERS_COMUNES + MAX_TESTERS_ESPECIALES; i++){
		ids_tester_disponibles[i] = true;
	}
	siguiente_id_tester_comun = 0; //idem
	siguiente_id_tester_especial = MAX_TESTERS_COMUNES; //idem
	   
    inicializado = true;
}


int *
get_id_dispositivo_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;
	if (!inicializado) inicializar();
	
	result = -1; //si no se consiguio id, como un mensaje de error
	int revisados = 0;
	int i;
	for (i = siguiente_id_dispositivo; revisados < MAX_DISPOSITIVOS_EN_SISTEMA && result == -1; i = (i + 1) % MAX_DISPOSITIVOS_EN_SISTEMA, revisados++){
		if (ids_dispositivos_disponibles[i]){
			result = i;
		}
	}
	siguiente_id_dispositivo = (siguiente_id_dispositivo + 1) % MAX_DISPOSITIVOS_EN_SISTEMA;
	
	if (result != -1){
		ids_dispositivos_disponibles[result] = false;
		result++;
	}
		
	return &result;
}

int *
get_id_tester_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	if (!inicializado) inicializar();

	result = -1;
	int tipo = *argp;
	
	int inicio, offset, cantidad, *siguiente;
	
	/* Para poder generalizar */
	if (tipo == TIPO_COMUN){
		inicio = siguiente_id_tester_comun;
		siguiente = &siguiente_id_tester_comun;
		offset = 0;
		cantidad = MAX_TESTERS_COMUNES;
	}else if (tipo == TIPO_ESPECIAL){
		inicio = siguiente_id_tester_especial;
		siguiente = &siguiente_id_tester_especial;
		offset = MAX_TESTERS_COMUNES;
		cantidad = MAX_TESTERS_ESPECIALES;
	}else{
		return &result;
	}
	
	int revisados = 0;
	int i;
	for (i = inicio; revisados < cantidad && result == -1; i = (i + 1) % cantidad + offset, revisados++){
		if (ids_tester_disponibles[i]){
			result = i;
		}
	}
	*siguiente = (*siguiente + 1) % cantidad + offset;
	
	if (result != -1){
		ids_tester_disponibles[result] = false;
		result++;
	}
	
	return &result;
}

int *
registrar_tester_activo_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	// Deprecated. Hay que ver de generar una nueva version en todo caso. Cuando se vea el tema de los nuevos servicios de localizacion
	return &result;
}

int *
devolver_id_dispositivo_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	if (!inicializado) inicializar();
	
	int id = *argp;	
	if (id <= 0 || id > MAX_DISPOSITIVOS_EN_SISTEMA ){
		result = -1;
		return &result;
	}
	
	if (ids_dispositivos_disponibles[id-1]){
		result = -2; //El dispositivo estaba habilitado, asi que nadie lo pidio en realidad como para poder devolverlo...
		return &result; 
	}
	
	ids_dispositivos_disponibles[id-1] = true;
	result = 1;
	
	return &result;
}

int *
devolver_id_tester_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	if (!inicializado) inicializar();
	
	int id = *argp;
	
	if (id <= 0 || id > MAX_TESTERS_COMUNES + MAX_TESTERS_ESPECIALES ){
		result = -1;
		return &result;
	}
	
	if (ids_tester_disponibles[id - 1]){
		result = -2; //El dispositivo estaba habilitado, asi que nadie lo pidio en realidad como para poder devolverlo...
		return &result; 
	}
	
	ids_tester_disponibles[id - 1] = true;
	result = 1;
	
	return &result;
}

