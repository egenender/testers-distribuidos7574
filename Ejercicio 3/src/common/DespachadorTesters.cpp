/* 
 * File:   DespachadorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTesters.h"
#include "Configuracion.h"

DespachadorTesters::DespachadorTesters( const Configuracion& config ) {
    
    key_t key = ftok( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero( Constantes::NombresDeParametros::MSGQUEUE_DESPACHADOR ) );
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    
    std::stringstream ss; //<DBG>
    ss << "MSGQUEUE_DESPACHADOR creada con id " << msgQueueId;
    Logger::notice( ss.str().c_str(), __FILE__ );
    ss.str("");
    
    if(this->msgQueueId == -1) {
	std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        throw error;
    }
    
}

DespachadorTesters::~DespachadorTesters() {
}

void DespachadorTesters::enviarOrden(int idDispositivo) {

    TMessageDespachador msg;
    msg.mtype = Constantes::ORDEN_TECNICO;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al despachador. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}
