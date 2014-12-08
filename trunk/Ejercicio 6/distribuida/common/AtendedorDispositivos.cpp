/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "../common/common.h"
#include <cstdlib>

AtendedorDispositivos::AtendedorDispositivos() { 
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS);
    this->cola_requerimiento = msgget(key, 0666);
    if(this->cola_requerimiento == -1) {
		exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS);
    this->cola_tests = msgget(key, 0666);
    if(this->cola_tests == -1) {
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
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
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
            
    int ret = msgsnd(this->cola_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirOrden(int idDispositivo, int* cantidad) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
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
                
    int ret = msgsnd(this->cola_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0); //REVISAR A QUE COLA MANDAR
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}
