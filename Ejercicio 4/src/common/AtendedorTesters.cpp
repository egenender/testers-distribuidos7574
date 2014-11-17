/* 
 * File:   AtendedorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 7, 2014, 8:55 PM
 */

#include "AtendedorTesters.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <sys/msg.h>
#include <cerrno>
#include <cstring>

AtendedorTesters::AtendedorTesters( const Configuracion& config ) {
    m_Key = ftok( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                  config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSG_QUEUE_ATENDEDOR) );
    m_MsgQueueId = msgget(m_Key, 0666 | IPC_CREAT);
    if(m_MsgQueueId == -1) {
        std::string err = std::string("Error al obtener la cola del atendedor de dispositivos. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
}

AtendedorTesters::~AtendedorTesters() {}

int AtendedorTesters::recibirRequerimiento() {

    return recibirReq(Constantes::MTYPE_REQUERIMIENTO);
}

int AtendedorTesters::recibirRequerimiento( int idTester ) {

    return recibirReq(Constantes::MTYPE_REQUERIMIENTO + idTester);
}

int AtendedorTesters::recibirReq( int tipoMsg ){
    TMessageAtendedor msg;
    int ret = msgrcv( m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), tipoMsg, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = idPrograma;
    
    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorTesters::recibirResultado(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.value;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

bool AtendedorTesters::destruirComunicacion() {

    return (msgctl(m_MsgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}
