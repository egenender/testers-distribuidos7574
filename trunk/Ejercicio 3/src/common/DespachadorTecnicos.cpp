/* 
 * File:   DespachadorTecnicos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTecnicos.h"
#include "Configuracion.h"

DespachadorTecnicos::DespachadorTecnicos( const Configuracion& config ) {

    key_t key = ftok( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_DESPACHADOR) );
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(this->msgQueueId == -1) {
        std::string error = std::string("Error creando la cola de mensajes del despachador. Errno = ") + std::string(strerror(errno));
        throw error;
    }
}

DespachadorTecnicos::~DespachadorTecnicos() {
}

int DespachadorTecnicos::recibirOrden() {

    TMessageDespachador msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageDespachador) - sizeof(long), 0, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;

}

bool DespachadorTecnicos::destruirComunicacion() {

    return (msgctl(this->msgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}

