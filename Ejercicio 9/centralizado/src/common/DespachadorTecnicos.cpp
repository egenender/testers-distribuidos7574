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
using std::string;

DespachadorTecnicos::DespachadorTecnicos( const Configuracion& config ) {
    const string ipcFileName = config.ObtenerParametroString(ARCHIVO_IPCS);
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    m_MsgQueueId = msgget(key, 0666 ); 
    if(m_MsgQueueId == -1) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
        exit(1);
    }
}

DespachadorTecnicos::~DespachadorTecnicos() {
}

int DespachadorTecnicos::recibirOrden() {

    TMessageDespachador msg;
    int ret = msgrcv(m_MsgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), MTYPE_ORDEN, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;

}
