/* 
 * File:   DespachadorTecnicos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTecnicos.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <sys/msg.h>
#include <cstring>
#include "errno.h"

DespachadorTecnicos::DespachadorTecnicos( const Configuracion& config ) {

    key_t key = ftok( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSG_QUEUE_DESPACHADOR) );
    m_MsgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(m_MsgQueueId == -1) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        throw error;
    }
}

DespachadorTecnicos::~DespachadorTecnicos() {
}

void DespachadorTecnicos::enviarOrden(int idDispositivo) {

    TMessageDespachador msg;
    msg.mtype = 1;
    msg.idDispositivo = idDispositivo;

    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al despachador. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int DespachadorTecnicos::recibirOrden() {

    TMessageDespachador msg;
    int ret = msgrcv(m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

bool DespachadorTecnicos::destruirComunicacion() {

    return (msgctl(m_MsgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}
