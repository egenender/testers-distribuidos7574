#include <stdio.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"

typedef struct broker_id_ip{
	int id;
	char ip[16];
}broker_id_ip_t;

broker_id_ip_t BROKERS[] = { 
	{1001, "192.168.1.104"},
	{1002, "192.168.1.102"},
	{1003, "192.168.1.101"}
};

#define ID_BROKER 1001
#define ID_NEXT_BROKER 1001

void crear_ipcs(){
	
	key_t key;	
	Logger::notice("Creo los semaforos y shm necesarias", __FILE__);
	Semaphore sem_comunes(SEM_CANT_TESTERS_COMUNES);
	sem_comunes.creaSem();
	sem_comunes.iniSem(0);
		
	Semaphore puedo_buscar(SEM_SHM_TESTERS_REQUERIMIENTO);
	puedo_buscar.creaSem();
	puedo_buscar.iniSem(1);
	
	key = ftok(ipcFileName.c_str(), SHM_TESTER_QUIERE_SHM);
    int shmtester = shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    int* tester_shm = (int*)shmat(shmtester, NULL, 0);   
    *tester_shm = 0;
    shmdt((void*)tester_shm);
    
    Semaphore sem_tester_shm(SEM_MUTEX_TESTER_QUIERE_SHM);
	sem_tester_shm.creaSem();
	sem_tester_shm.iniSem(1);
	
	key = ftok(ipcFileName.c_str(), SHM_NEXT_BROKER);
    int shmnext = shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    int* next = (int*)shmat(shmnext, NULL, 0);   
	*next = ID_NEXT_BROKER;
	shmdt((void*) next);
	
	Semaphore sem_next(SEM_MUTEX_NEXT_BROKER);
	sem_next.creaSem();
	sem_next.iniSem(1);
	
	/* Pongo a circular la shm de testers*/
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_SHM_TESTERS);
	int cola_shm_testers = msgget(key, 0660 | IPC_CREAT);
	
	TMessageAtendedor msg;
	for (int i = 0; i < CANT_RESULTADOS; i++){
		msg.resultados[i].resultadosPendientes = 0;
		msg.resultados[i].idDispositivo = 0;
		msg.resultados[i].resultadosGraves = 0;
    }
	msg.mtype = ID_BROKER;
	
	msg.tabla.start = msg.tabla.end = msg.tabla.cant = 0;
    for (int i = 0; i < MAX_TESTERS_ESPECIALES; i++){
		msg.tabla.testers_especiales[i] = 0;
		Semaphore sem_especial(SEM_ESPECIAL_DISPONIBLE + i);
		sem_especial.creaSem();
		sem_especial.iniSem(0);
	}
	
	msg.version = 1;
	msg.finalizar_conexion = 0;
	
	if (ID_BROKER == 1001){ //Solo el "lider" pone a circular
		int ret = msgsnd(cola_shm_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if (ret == -1) exit(0);
	}
}

void crear_sub_brokers(){	
	char param_id[5];
	sprintf(param_id, "%d", ID_BROKER);
	
	Logger::notice("Creo el broker pasa manos", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_pasa_manos", "broker_pasa_manos", (char*)0);
		Logger::notice ("Algo se rompio", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el broker de nuevos requerimientos", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_requerimientos", "broker_requerimientos", param_id, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker de requerimientos para testers especiales", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_req_especiales", "broker_req_especiales", (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker de requerimientos de shm de testers", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_shm_testers_req", "broker_shm_testers_req", (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker de otorgamiento de shm de testers", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_shm_testers_otorga", "broker_shm_testers_otorga", param_id,(char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker de disponibilidad de testers", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_disponibilidad_testers", "broker_disponibilidad_testers", param_id, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker pasa manos a otros brokers (testers)", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_pasa_brokers_testers", "broker_pasa_brokers_testers", param_id, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker pasa manos a otros brokers (dispositivos)", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_pasa_brokers_dispositivos", "broker_pasa_brokers_dispositivos", param_id, (char*)0);
        exit(1);
	}
	
	if (fork() == 0){
		char param_mtype[4];
		sprintf(param_mtype, "%d", MTYPE_REQUERIMIENTO_SHM_TESTERS);
		char param_cola[4];
		sprintf(param_cola, "%d", MSGQUEUE_BROKER_REQUERIMIENTOS_SHM);
		execlp("./broker/broker_cola_shm", "broker_cola_shm", param_mtype,param_cola ,(char*)0);
        exit(1);
	}
	
	if (fork() == 0){
		char param_mtype[4];
		sprintf(param_mtype, "%d", MTYPE_DEVOLUCION_SHM_TESTERS);
		char param_cola[4];
		sprintf(param_cola, "%d", MSGQUEUE_BROKER_DEVOLUCION_SHM);
		execlp("./broker/broker_cola_shm", "broker_cola_shm",param_mtype,param_cola,(char*)0);
        exit(1);
	}
}

void crear_servers(){
	
	char param_id[10];
    sprintf(param_id, "%d", ID_BROKER);
    char param_cola[10];
    char param_cola2[10];
    
    // HACIA TESTERS
    sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
    sprintf(param_cola2, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS_FINAL);
    Logger::notice("Creo el servidor receptor de mensajes de testers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_TESTERS , param_id, param_cola,param_cola2 ,(char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el servidor emisor de mensajes a testers", __FILE__);
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS_FINAL);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_TESTERS ,param_id, param_cola,(char*)0);
        exit(1);
	}
	
	// HACIA DISPOSITIVOS
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
    sprintf(param_cola2, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS_FINAL);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , param_id, param_cola, param_cola2, (char*)0);
        exit(1);
	}
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS_FINAL);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS ,param_id, param_cola,(char*)0);
        exit(1);
	}
	
	// Hacia otros brokers, para la shm
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_SHM_TESTERS);
	Logger::notice("Creo el servidor receptor de mensajes de testers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_SHM , "1003", param_cola,"0",(char*)0);
        exit(1);
	}
}

void crear_clientes_a_brokers(){
	//TODO ESTO DEBERIA ESTAR EN UN FOR, y probablemente habria que pedir los ids del servicio rpc
	char param_cola_dispositivos[10];
	sprintf(param_cola_dispositivos, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS_FINAL);
	char param_cola_testers[10];
	sprintf(param_cola_testers, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS_FINAL);
	char param_cola_shm[10];
	sprintf(param_cola_shm, "%d", MSGQUEUE_BROKER_SHM_TESTERS);
	
	for (size_t i = 0; i < sizeof(BROKERS); i++){
		if (BROKERS[i].id == ID_BROKER) continue; //no tiene sentido conectarse con uno mismo
		
		char param_id[10];
		sprintf(param_id, "%d", BROKERS[i].id);
		
		if (fork() == 0){													//Se hace pasar por un tester que viene "de mas alla"
			execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",BROKERS[i].ip ,PUERTO_SERVER_RECEPTOR_TESTERS , param_id, param_cola_dispositivos, "0",(char*)0);
			exit(1);
		}
	
		if (fork() == 0){													//Se hace pasar por un dispositivo que viene "de mas alla"
			execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",BROKERS[i].ip ,PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , param_id, param_cola_testers, "0",(char*)0);
			exit(1);
		}
	
		if (fork() == 0){													
			execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",BROKERS[i].ip ,PUERTO_SERVER_RECEPTOR_SHM , param_id, param_cola_shm, "0",(char*)0);
			exit(1);
		}		
	}
}

int main (void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crear_ipcs();
	crear_servers();
	crear_sub_brokers();
	
	sleep(5);
	crear_clientes_a_brokers();
	exit(0);
}

