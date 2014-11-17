/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */

#include "AtendedorDispositivos.h"
#include "logger/Logger.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include <sys/msg.h>
#include <cerrno>
#include <cstring>

AtendedorDispositivos::AtendedorDispositivos( const Configuracion& config ) {
    m_Key = ftok( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ).c_str(),
                  config.ObtenerParametroEntero( Constantes::NombresDeParametros::MSG_QUEUE_ATENDEDOR ) );
    m_MsgQueueId = msgget(m_Key, 0666 | IPC_CREAT);
    if(m_MsgQueueId == -1) {
        throw std::string("Error al obtener la cola del atendedor de dispositivos. Errno: " + errno);
    }
}

AtendedorDispositivos::~AtendedorDispositivos() {}

void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {
    enviarReq( Constantes::MTYPE_REQUERIMIENTO, idDispositivo ); //Para el primero que lo agarre
}

void AtendedorDispositivos::enviarRequerimiento(int idTester, int idDispositivo) {
    enviarReq( Constantes::MTYPE_REQUERIMIENTO + idTester, idDispositivo ); //Para uno especifico
}

void AtendedorDispositivos::enviarReq( int tipoMsg, int idDisp ){
    TMessageAtendedor msg;
    msg.mtype = tipoMsg;
    msg.idDispositivo = idDisp;

    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
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
    
    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(m_MsgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.value;
}

bool AtendedorDispositivos::destruirComunicacion() {

    return (msgctl(m_MsgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}
