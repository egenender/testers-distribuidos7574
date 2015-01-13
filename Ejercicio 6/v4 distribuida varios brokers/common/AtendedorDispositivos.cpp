/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "../common/common.h"
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int getIdDispositivo();
void devolverIdDispositivo(int);

AtendedorDispositivos* at;
pid_t receptor;

void terminar(int sig){
	at->terminar_atencion();
	kill(receptor, SIGHUP);
	wait(NULL);
	execlp("./Dispositivo", "",(char*)0);
	exit(1);
}

void restart_padre(pid_t pid){
	sleep(TIEMPO_ESPERA_RESTART);
	kill(pid, SIGHUP);
	exit(0);	
}

AtendedorDispositivos::AtendedorDispositivos() { 	
	at = this;
	this->idDispositivo = getIdDispositivo();
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS_ENVIOS);
    this->cola_envios = msgget(key, 0666);
    if(this->cola_envios == -1) {
		exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS_RECIBOS);
    this->cola_recibos = msgget(key, 0666);
    if(this->cola_recibos == -1) {
        exit(1);
    }
    
    signal(SIGHUP, terminar);
    
    char param_id[10];
    sprintf(param_id, "%d", this->idDispositivo);
    char param_cola[10];
    sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS_RECIBOS);
    receptor = fork();
    if (receptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",UBICACION_BROKER ,PUERTO_SERVER_EMISOR_DISPOSITIVOS , param_cola,(char*)0);
        exit(1);
	}
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS_ENVIOS);
	
	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",UBICACION_BROKER ,PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , param_id, param_cola, param_pid,(char*)0);
        exit(1);
	}
}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento() {

    TMessageAtendedor msg;
    msg.mtype = this->idDispositivo;
    msg.mtype_envio = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.es_requerimiento = 1;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma() {
    TMessageAtendedor msg;
    
    pid_t hijo = fork();
    if (hijo == 0){
		restart_padre(getppid());
	}
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    kill(hijo, SIGINT);
    wait(NULL);
    this->ultimoTester = msg.tester;
    return msg.value;

}
void AtendedorDispositivos::enviarResultado(int resultado) {
    TMessageAtendedor msg;
    msg.mtype = this->idDispositivo;
    msg.mtype_envio = (long) this->ultimoTester;
    msg.finalizar_conexion = 0;
    msg.tester = this->ultimoTester;
    msg.idDispositivo = idDispositivo;
    msg.value = resultado;
    msg.es_requerimiento = 0;
            
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirOrden(int* cantidad) {

    TMessageAtendedor msg;
    pid_t hijo = fork();
    if (hijo == 0){
		restart_padre(getppid());
	}
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    kill(hijo, SIGINT);
    wait(NULL);
    *cantidad = msg.cant_testers;
    return msg.value;

}

void AtendedorDispositivos::terminar_atencion(){
	devolverIdDispositivo(this->idDispositivo);
	TMessageAtendedor msg;
	msg.mtype = this->idDispositivo;
    msg.mtype_envio = this->idDispositivo;
    msg.finalizar_conexion = FINALIZAR_CONEXION;
                
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::obtenerIdDispositivo(){
	return this->idDispositivo;
}


int getIdDispositivo(){
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_GETTING_IDS);
    int cola_ids = msgget(key, 0666 | IPC_CREAT);
    
	if (fork() == 0){
		execlp("./servicio_rpc/get_id", "get_id", UBICACION_SERVER_RPC ,"2",(char*)0);
		printf("ALGO NO ANDUVO\n");
        exit(1);
	}
		
	wait(NULL);
	
	TMessageAtendedor msg;
    int ret = msgrcv(cola_ids, &msg, sizeof(TMessageAtendedor) - sizeof(long), 3, 0);
    if (ret == -1){
		//No se pudo conseguir id
		exit(-1);
	}
	
	int id = msg.value;

	if (id <= 0){
		//Algo salio mal, no quedan ids, o lo que fuere
		exit(id);
	}
	return id;
}

void devolverIdDispositivo(int id){
	char param_id[5];
	sprintf(param_id, "%d", id);
	
	if (fork() == 0){
		execlp("./servicio_rpc/devolver_id", "devolver_id", UBICACION_SERVER_RPC ,"2", param_id,(char*)0);
		printf("ALGO NO ANDUVO\n");
        exit(1);
	}
		
	wait(NULL);
}
