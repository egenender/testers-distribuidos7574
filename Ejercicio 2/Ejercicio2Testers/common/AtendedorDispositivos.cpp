/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */

#include "AtendedorDispositivos.h"

AtendedorDispositivos::AtendedorDispositivos() { 
    this->key = ftok(ipcFileName.c_str(), MSG_QUEUE_DESPACHADOR);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT);
    if(this->msgQueueId == -1) {
        throw std::string("Error al obtener la cola del atendedor de dispositivos. Errno: " + errno);
    }
}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error("Error al recibir requerimiento del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

void AtendedorDispositivos::enviarPrograma(int idDispositivo, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = idPrograma;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar programa al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.value;

}
void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = resultado;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorDispositivos::recibirResultado(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir resultado del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.value;

}

void AtendedorDispositivos::enviarOrden(int idDispositivo, int orden) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar orden al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.value;

}