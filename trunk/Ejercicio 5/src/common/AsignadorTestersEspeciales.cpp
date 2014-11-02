/* 
 * File:   DespachadorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "DespachadorTesters.h"
#include "../logger/Logger.h"

AsignadorTestersEspeciales::AsignadorTestersEspeciales() {

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DESPACHADOR);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(this->msgQueueId == -1) {
		std::string error = std::string("Error creando la cola de mensajes del asignador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
    }
    
}

AsignadorTestersEspeciales::AsignadorTestersEspeciales(const AsignadorTestersEspeciales& orig) {
}

AsignadorTestersEspeciales::~AsignadorTestersEspeciales() {
}

void AsignadorTestersEspeciales::asignar(int idDispositivo, list<int> testersEspecialesIds) {

	for (int i = 0; i < testersEspecialesIds.size() ; i++) {
		TMessageAsignador msg;
		msg.mtype = testersEspecialesIds[i];
		msg.idDispositivo = idDispositivo;
    
		int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAsignador) - sizeof(long), 0);
		if (ret == -1) {
			std::string error = std::string("Error al enviar orden al despachador. Error: ") + std::string(strerror(errno));
			Logger::error(error.c_str(), __FILE__);
			throw error;
		}
	}

	for ()
}
