#include "registracion.h"
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
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
	
    key_t key = ftok("/tmp/buchwaldipcs", SHM_TABLA_TESTERS);
    int shmtabla = shmget(key, sizeof(tabla_testers_disponibles_t) , IPC_CREAT | 0660);
    tabla = (tabla_testers_disponibles_t*)shmat(shmtabla, NULL, 0);
    
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
	result = 1;
	int id = *argp;
	
	if (id <= 0 || id > MAX_TESTERS_ESPECIALES + MAX_TESTERS_COMUNES){
		result = -1;
		return &result;
	}
	
	
	if (ids_tester_disponibles[id-1]){
		result = -2; //Si no lo pidio, como lo va a registrar?
		return &result;
	}
	
	/* OJO con esto, que es bloqueante, hay que ver si hay alguna otra forma de resolverlo */
	key_t key = ftok("/tmp/buchwaldipcs",SEM_TABLA_TESTERS);
	int semid = semget(key,1,0660);
		
	//sem_tabla.p();
	struct sembuf oper;
    oper.sem_num = 0;
    oper.sem_op = -1;
    oper.sem_flg = 0;
    semop(semid,&oper,1);
	oper.sem_op = 1;
	
	if (id <= MAX_TESTERS_COMUNES){ //es id correspondiente a un tester comun
		printf("Era un id de tester comun\n");
		tabla->testers_comunes[tabla->end++] = id;
		tabla->cant++;
		
		//sem_comunes.v();
		key = ftok("/tmp/buchwaldipcs",SEM_CANT_TESTERS_COMUNES);
		int semcomunes = semget(key,1,0660);
		semop(semcomunes,&oper,1);
			
	}else{ //es id correspondiente a un tester especial
		printf("Era un id de tester especial\n");
		tabla->testers_especiales[id - MAX_TESTERS_COMUNES - 1] = true;
		
		key = ftok("/tmp/buchwaldipcs",SEM_ESPECIAL_DISPONIBLE + (id - MAX_TESTERS_COMUNES - 1));
		int sem_especial = semget(key,1,0660);
		semop(sem_especial,&oper,1);
		//Semaphore sem_especial(SEM_ESPECIAL_DISPONIBLE + (id - MAX_TESTERS_COMUNES - 1));
		//sem_especial.v();
	}
		
	//sem_tabla.v();
	oper.sem_op = 1;
	semop(semid,&oper,1);
	
	return &result;
}

int terminar_conexion(int id, int idcola){
	key_t key = ftok("/tmp/buchwaldipcs", idcola);
	int cola = msgget(key, 0666);
	TMessageAtendedor msg;
	msg.mtype = id;
	msg.finalizar_conexion = FINALIZAR_CONEXION;
	msgsnd(cola, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
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
	
	//Mando mensaje de finalizacion para terminar la conexion con este dispositivo
	terminar_conexion(id, MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	
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
	
	//Mando mensaje de finalizacion para terminar la conexion con este tester
	terminar_conexion(id, MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	
	result = 1;
	return &result;
}

