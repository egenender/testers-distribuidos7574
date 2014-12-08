/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "../common/common.h"
#include <cstdlib>

AtendedorDispositivos::AtendedorDispositivos(int idDispositivo) { 
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
    
       
    char param_id[10];
    sprintf(param_id, "%d", idDispositivo);
    char param_cola[10];
    sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS_ENVIOS);
    pid_t receptor = fork();
    if (receptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",UBICACION_SERVER ,PUERTO_SERVER_EMISOR , param_id,(char*)0);
        exit(1);
	}
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	
	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",UBICACION_SERVER ,PUERTO_SERVER_RECEPTOR , param_id, param_cola, param_pid,(char*)0);
        exit(1);
	}
}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.cola_a_usar = MSGQUEUE_TESTERS_RECIBOS;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->ultimoTester = msg.tester;
    return msg.value;

}
void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {
    TMessageAtendedor msg;
    msg.mtype = (long) this->ultimoTester;
    msg.finalizar_conexion = 0;
    msg.tester = this->ultimoTester;
    msg.idDispositivo = idDispositivo;
    msg.value = resultado;
    msg.cola_a_usar = MSGQUEUE_TESTERS_RECIBOS;
            
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirOrden(int idDispositivo, int* cantidad) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    *cantidad = msg.cant_testers;
    return msg.value;

}

void AtendedorDispositivos::terminar_atencion(){
	TMessageAtendedor msg;
    msg.mtype = (long) this->ultimoTester;
    msg.finalizar_conexion = FINALIZAR_CONEXION;
                
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}
