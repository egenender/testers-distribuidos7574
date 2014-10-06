/* 
 * File:   DespachadorTecnicos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTecnicos.h"

DespachadorTecnicos::DespachadorTecnicos() {
    
    key_t key = ftok(ipcFileName.c_str(), MSG_QUEUE_DESPACHADOR);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(this->msgQueueId == -1) {
        throw std::string("Error creando la cola de mensajes del despachador. Errno = " + errno);
    }
    
}

DespachadorTecnicos::DespachadorTecnicos(const DespachadorTecnicos& orig) {
}

DespachadorTecnicos::~DespachadorTecnicos() {
}

void DespachadorTecnicos::enviarOrden(int idDispositivo) {

    TMessageDespachador msg;
    msg.mtype = 0;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar orden al despachador. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int DespachadorTecnicos::recibirOrden() {

    TMessageDespachador msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0, 0);
    if(ret == -1) {
        std::string error("Error al recibir requerimiento del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;

}