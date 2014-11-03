/* 
 * File:   AsignadorTestersEspecialesB.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "AsignadorTestersEspecialesB.h"
#include "../logger/Logger.h"
#include "common.h"
#include <cstdlib>

AsignadorTestersEspecialesB::AsignadorTestersEspecialesB() {

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DESPACHADOR);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(this->msgQueueId == -1) {
		std::string error = std::string("Error creando la cola de mensajes del asignador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
    }
}

AsignadorTestersEspecialesB::AsignadorTestersEspecialesB(const AsignadorTestersEspecialesB& orig) {
}

AsignadorTestersEspecialesB::~AsignadorTestersEspecialesB() {
}

int AsignadorTestersEspecialesB::recibirPedido(int idTesterEspecial) {

	TMessageAsignador msg;
	int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAsignador) - sizeof(long), idTesterEspecial, 0);
	if (ret == -1) {
		std::string error = std::string("Error al recibir pedido para testers especiales. Error: ") + std::string(strerror(errno));
		Logger::error(error.c_str(), __FILE__);
		throw error;
		return -1;
	}
	return msg.idDispositivo;
}

void AsignadorTestersEspecialesB::enviarResultadoAlTerminar(int idTesterEspecial, int idDispositivo, int resultado) {

	TMessageAsignador msg;
	msg.mtype = (long)idTesterEspecial;
	msg.idDispositivo = idDispositivo;
	msg.value = resultado;
	int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAsignador) - sizeof(long), 0);
	if (ret == -1) {
		std::string error = std::string("Error al enviar resultado parcial del tester Especial. Error: ") + std::string(strerror(errno));
		Logger::error(error.c_str(), __FILE__);
		throw error;
		exit(0);
	}
}

