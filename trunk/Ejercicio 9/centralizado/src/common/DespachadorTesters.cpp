/* 
 * File:   DespachadorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTesters.h"
#include "../common/Configuracion.h"
#include "../logger/Logger.h"

using namespace Constantes::NombresDeParametros;

DespachadorTesters::DespachadorTesters( const Configuracion& config ) {
    key_t key = ftok( config.ObtenerParametroString( ARCHIVO_IPCS ).c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    m_MsgQueueId = msgget(key, 0666 );
    if(m_MsgQueueId == -1) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
    }
}

DespachadorTesters::DespachadorTesters(const DespachadorTesters& orig) {
}

DespachadorTesters::~DespachadorTesters() {
}

void DespachadorTesters::enviarOrden(int idDispositivo) {
    TMessageDespachador msg;
    msg.mtype = MTYPE_ORDEN;
    msg.idDispositivo = idDispositivo;
    int ret = msgsnd(m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al despachador. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}
