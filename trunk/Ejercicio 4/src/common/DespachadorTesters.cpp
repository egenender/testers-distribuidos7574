/* 
 * File:   DespachadorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTesters.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <sys/msg.h>
#include <cerrno>
#include <cstring>

DespachadorTesters::DespachadorTesters() {

    key_t key = ftok(Constantes::ARCHIVO_IPCS.c_str(), Constantes::MSG_QUEUE_DESPACHADOR);
    m_MsgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(m_MsgQueueId == -1) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        throw error;
    }
}

DespachadorTesters::~DespachadorTesters() {}

void DespachadorTesters::enviarOrden(int idDispositivo) {

    TMessageDespachador msg;
    msg.mtype = 0;
    msg.idDispositivo = idDispositivo;

    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al despachador. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}
