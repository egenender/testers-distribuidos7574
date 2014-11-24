/* 
 * File:   AtendedorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 7, 2014, 8:55 PM
 */

#include "AtendedorTesters.h"

AtendedorTesters::AtendedorTesters() {
    this->key = ftok(ipcFileName.c_str(), MSG_QUEUE_ATENDEDOR);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT);
    if(this->msgQueueId == -1) {
	std::string err = std::string("Error al obtener la cola del atendedor de dispositivos. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig) {
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int idPrograma, int idTester) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = idTester;
    msg.value = idPrograma;
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

int AtendedorTesters::recibirResultado(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
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
    
    int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

bool AtendedorTesters::destruirComunicacion() {

    return (msgctl(this->msgQueueId, IPC_RMID, (struct msqid_ds*)0) != -1);
}
