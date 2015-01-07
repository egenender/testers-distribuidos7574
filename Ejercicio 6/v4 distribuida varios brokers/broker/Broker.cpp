#include <stdio.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"

#define ID_BROKER 1

void crear_ipcs(){
	Logger::notice("Creo las colas necesarias", __FILE__);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ATENCION_REQUERIMIENTOS_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
		
	Logger::notice("Creo los semaforos y shm necesarias", __FILE__);
	Semaphore sem_comunes(SEM_CANT_TESTERS_COMUNES);
	sem_comunes.creaSem();
	sem_comunes.iniSem(0);
	
	key = ftok(ipcFileName.c_str(), SHM_TABLA_TESTERS);
    int shmtabla = shmget(key, sizeof(tabla_testers_disponibles_t) , IPC_CREAT | 0660);
    tabla_testers_disponibles_t* tabla = (tabla_testers_disponibles_t*)shmat(shmtabla, NULL, 0);
    
    tabla->start = tabla->end = tabla->cant = 0;
    for (int i = 0; i < MAX_TESTERS_ESPECIALES; i++){
		tabla->testers_especiales[i] = 0;
		Semaphore sem_especial(SEM_ESPECIAL_DISPONIBLE + i);
		sem_especial.creaSem();
		sem_especial.iniSem(0);
	}
	shmdt((void*)tabla);
	
	Semaphore sem_tabla(SEM_TABLA_TESTERS);
	sem_tabla.creaSem();
	sem_tabla.iniSem(1);
	
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
	*next = ID_BROKER; //por ahora es el broker 1, y listo TODO
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
	msg.version = 1;
	
	int ret = msgsnd(cola_shm_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
	if (ret == -1) exit(0);
}

void crear_sub_brokers(){
	Logger::notice("Creo el servidor rpc", __FILE__);
	if (fork() == 0){
		execlp("./broker/servicio_rpc/registracion_server", "registracion_server", (char*)0);
		Logger::notice ("Algo se rompio", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el broker pasa manos", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_pasa_manos", "broker_pasa_manos", (char*)0);
		Logger::notice ("Algo se rompio", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el broker de nuevos requerimientos", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_requerimientos", "broker_requerimientos", (char*)0);
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
		char param_id[2];
		sprintf(param_id, "%d", ID_BROKER);
		execlp("./broker/broker_shm_testers_otorga", "broker_shm_testers_otorga", param_id,(char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el broker de disponibilidad de testers", __FILE__);
	if (fork() == 0){
		execlp("./broker/broker_disponibilidad_testers", "broker_disponibilidad_testers", (char*)0);
        exit(1);
	}
}

void crear_servers(){
	
	char param_id[10];
    sprintf(param_id, "%d", 1);
    char param_cola[10];
    char param_cola2[10];
    
    // HACIA TESTERS
    sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
    sprintf(param_cola2, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
    Logger::notice("Creo el servidor receptor de mensajes de testers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_TESTERS , param_id, param_cola,param_cola2 ,(char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el servidor emisor de mensajes a testers", __FILE__);
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_TESTERS ,param_id, param_cola,(char*)0);
        exit(1);
	}
	
	// HACIA DISPOSITIVOS
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
    sprintf(param_cola2, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , param_id, param_cola, param_cola2, (char*)0);
        exit(1);
	}
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS ,param_id, param_cola,(char*)0);
        exit(1);
	}
}

int main (void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crear_ipcs();
	crear_servers();
	crear_sub_brokers();
	exit(0);
}

