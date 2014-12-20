#include <stdio.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"

void crear_ipcs(){
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ATENCION_REQUERIMIENTOS_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	msgget(key, 0660 |IPC_CREAT);
	
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
	
	Semaphore sem_tabla(SEM_TABLA_TESTERS);
	sem_tabla.creaSem();
	sem_tabla.iniSem(1);
}

void crear_sub_brokers(){
	if (fork() == 0){
		execlp("./broker/broker_pasa_manos", "broker_pasa_manos", (char*)0);
        exit(1);
	}
	
	if (fork() == 0){
		execlp("./broker/broker_requerimientos", "broker_requerimientos", (char*)0);
        exit(1);
	}
	
	if (fork() == 0){
		execlp("./broker/broker_req_especiales", "broker_req_especiales", (char*)0);
        exit(1);
	}

}

void crear_servers(){
	
	char param_id[10];
    sprintf(param_id, "%d", 1);
    char param_cola[10];
    
    // HACIA TESTERS
    sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
    
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_TESTERS , param_id, param_cola, (char*)0);
        exit(1);
	}
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_TESTERS ,param_cola, param_id,(char*)0);
        exit(1);
	}
	
	// HACIA DISPOSITIVOS
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
    
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , param_id, param_cola, (char*)0);
        exit(1);
	}
	
	sprintf(param_cola, "%d", MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS ,param_cola, param_id,(char*)0);
        exit(1);
	}
}

int main (void){
	crear_ipcs();
	crear_servers();
	crear_sub_brokers();
	exit(0);
}

