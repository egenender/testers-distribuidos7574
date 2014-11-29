/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "../common/common.h"
#include <cstdlib>
#include "../logger/Logger.h"
#include <string>

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
    this->ultimoTester = 0;
}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor* msg = (TMessageAtendedor*) malloc (sizeof(TMessageAtendedor));;
    int ret = msgrcv(this->cola_requerimiento, msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    std::stringstream ss;
    ss << "El dispositivo "<< idDispositivo << " recibe programa desde tester " << msg->tester;
    Logger::notice(ss.str(), __FILE__);
    this->ultimoTester = msg->tester;
    int valor = msg->value;
    free(msg);
    return valor;

}
void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    resultado_test_t resultado_test;
    
    resultado_test.tester = this->ultimoTester;
    std::stringstream ss;
    ss << "El atendedor le manda resultado del dispositivo " << idDispositivo << " al tester " << ultimoTester;
    Logger::notice(ss.str(), __FILE__);
    resultado_test.result = resultado;
    resultado_test.dispositivo = idDispositivo;
            
    int ret = msgsnd(this->cola_tests, &resultado_test, sizeof(resultado_test_t) - sizeof(long), 0);
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
