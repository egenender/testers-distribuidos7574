/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "../common/common.h"
#include <cstdlib>
#include <string>

AtendedorDispositivos::AtendedorDispositivos(int idDispositivo) { 
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_NUEVO_REQUERIMIENTO);
    this->cola_requerimiento = msgget(key, 0666);
    if(this->cola_requerimiento == -1) {
		exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ESCRITURA_RESULTADOS);
    this->cola_tests = msgget(key, 0666);
    if(this->cola_tests == -1) {
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS_LECTURA);
    this->cola_dispositivos = msgget(key, 0666);
    if(this->cola_dispositivos == -1) {
        exit(1);
    }
    
    std::stringstream ss;
    ss << idDispositivo;
    
    Logger::notice("Voy a ejecutar tcpclient_receptor", __FILE__);
    if (fork() == 0){
		execlp("./tcpclient_receptor", "tcpclient_receptor", "localhost", "9000", ss.str().c_str(), (char*)0);
		Logger::error("Error al ejecutar el programa tcpclient_receptor", __FILE__);
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
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->ultimoTester = msg.idDispositivo;
    return msg.value;

}
void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    resultado_test_t resultado_test;
    
    resultado_test.tester = (long)this->ultimoTester;
    resultado_test.result = resultado;
    resultado_test.dispositivo = idDispositivo;
            
    int ret = msgsnd(this->cola_tests, &resultado_test, sizeof(resultado_test_t) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;

}
