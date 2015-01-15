#include "semaforos_distribuidos.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <cstdlib>


void semaforoDistribuido_P(tabla_testers_disponibles_t* tabla, int id_sub_broker){
	key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS_FINAL);
	int cola_recibos = msgget(key, 0666);
        
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_SHM);
    int cola_envios = msgget(key, 0666);
    	
	TMessageAtendedor msg;
	msg.mtype = MTYPE_REQUERIMIENTO_SHM_TESTERS;
	msg.mtype_envio = id_sub_broker;
	msg.finalizar_conexion = 0;
	msg.es_requerimiento = 1;
	msg.tester = id_sub_broker;
	
	if (msgsnd(cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0) == -1){
		exit(0);
	}
	
    if (msgrcv(cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), id_sub_broker, 0) == 1){
		exit(0);
    }
    
    //memcpy(resultados, msg.resultados, sizeof(msg.resultados));
    //copiar(resultados, msg.resultados);
    memcpy(tabla, &msg.tabla, sizeof(tabla_testers_disponibles_t));
}

void semaforoDistribuido_V(tabla_testers_disponibles_t* tabla, int id_sub_broker){
	TMessageAtendedor msg;
	msg.mtype = MTYPE_DEVOLUCION_SHM_TESTERS;
	msg.mtype_envio = MTYPE_DEVOLUCION_SHM_TESTERS;
	msg.finalizar_conexion = 0;
	msg.es_requerimiento = 1;
	msg.tester = id_sub_broker;
	//memcpy(msg.resultados, resultados, sizeof(resultados));
	//copiarResultado(msg.resultados, resultados);
	memcpy( &msg.tabla,tabla, sizeof(tabla_testers_disponibles_t));
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_DEVOLUCION_SHM);
    int cola_envios = msgget(key, 0666);
   
	if (msgsnd(cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0) == -1 ){
		exit(0);
	} 
}


