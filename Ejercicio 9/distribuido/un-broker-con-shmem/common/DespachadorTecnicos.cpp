/* 
 * File:   DespachadorTecnicos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTecnicos.h"
#include "Configuracion.h"
#include <cstdlib>
#include "../logger/Logger.h"

using namespace Constantes::NombresDeParametros;

DespachadorTecnicos::DespachadorTecnicos( const Configuracion& config ) {
    key_t key = ftok( config.ObtenerParametroString(ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    m_MsgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if( m_MsgQueueId == -1 ) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
        exit(1);
    }
}

DespachadorTecnicos::DespachadorTecnicos(const DespachadorTecnicos& orig) {
}

DespachadorTecnicos::~DespachadorTecnicos() {
}

int DespachadorTecnicos::recibirOrden() {
    TMessageDespachador msg;
    int ret = msgrcv( m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), MTYPE_ORDEN, 0 );
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;

}

bool DespachadorTecnicos::destruirComunicacion() {
    return (msgctl(m_MsgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}
