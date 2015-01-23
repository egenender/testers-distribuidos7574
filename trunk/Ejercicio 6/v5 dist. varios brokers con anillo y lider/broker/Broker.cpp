#include <stdio.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include "ids_brokers.h"

#define ID_BROKER 1002

void crear_ipcs(int master){
	
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
    shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    	
	Semaphore sem_next(SEM_MUTEX_NEXT_BROKER);
	sem_next.creaSem();
	sem_next.iniSem(1);
	
	Semaphore sem_anillo(SEM_ANILLO_FORMANDO);
	sem_anillo.creaSem();
	sem_anillo.iniSem(0);
	
	key = ftok(ipcFileName.c_str(), SHM_VERSION);
    int shmversion = shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    int* version_id = (int*)shmat(shmversion, NULL, 0);   
    *version_id = ID_BROKER;
    shmdt((void*)version_id);
	
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
	
	key = ftok(ipcFileName.c_str(), SHM_LIDER);
    int shmlider = shmget(key, sizeof(int) , 0660 | IPC_CREAT);
    int* soy_lider = (int*)shmat(shmlider, NULL, 0);   
    *soy_lider = 0;
	
	std::string programa;
	
	if ( master ) { //FIX condicion del if por algo por parametro
		sleep(3);
		programa = "sender";		
	} else {
		programa = "listener";
	}
	
	std::string ubicacion = "./Anillo/";
	std::string ejecucion = ubicacion + programa;
	std::stringstream id;
	id << ID_BROKER;
	
	if (fork() == 0){
		execlp(ejecucion.c_str(), programa.c_str(), id.str().c_str(),(char*)0);
		exit(1);
	}
	
	sem_anillo.p();
	wait(NULL);
	//Logger::notice("Se termino de armar el anillo, esperando a cierre de conexion para finalizar configuracion", __FILE__);
	sleep(20);
	
	if (fork() == 0){
		execlp("./Anillo/listener", "listener",(char*)0);
		exit(1);
	}
	
	if (*soy_lider){ //Solo el "lider" pone a circular
		int ret = msgsnd(cola_shm_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if (ret == -1) exit(0);
	}
	shmdt((void*) soy_lider);
	shmdt((void*)tester_shm);
	
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
	
	for (size_t i = 0; i < CANT_BROKERS; i++){
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

int main (int argc, char** argv){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crear_servers();
	crear_ipcs(argc == 2 && atoi(argv[1]) );
	crear_sub_brokers();
	
	sleep(1);
	crear_clientes_a_brokers();
	exit(0);
}

