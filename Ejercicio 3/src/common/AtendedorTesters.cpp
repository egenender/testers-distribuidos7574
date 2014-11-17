#include "AtendedorTesters.h"

AtendedorTesters::AtendedorTesters() {
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_NUEVO_REQUERIMIENTO);
    this->cola_requerimiento = msgget(key, 0666 | IPC_CREAT);
    if(this->msgQueueId == -1) {
	std::string err = std::string("Error al obtener la cola del atendedor de testers. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_LECTURA_RESULTADOS);
    this->cola_recibos_tests = msgget(key, 0666 | IPC_CREAT);
    if(this->msgQueueId == -1) {
        msgctl(this->cola_requerimiento, IPC_RMID, (struct msqid_ds*)0);
	std::string err = std::string("Error al obtener la cola del atendedor de testers. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
    
}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig) {
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

int AtendedorTesters::recibir2doRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO_SEGUNDO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = tester; //FIXME?
    msg.value = idPrograma;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

resultado_test_t AtendedorTesters::recibirResultado(int idTester) {

    resultado_test_t rsp;
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos_tests, &msg, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    rsp.tester = msg.mtype;
    rsp.dispositivo = msg.idDispositivo;
    rsp.result = msg.value;
    
    return rsp;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

bool AtendedorTesters::destruirComunicacion() {

    return (msgctl(this->cola_recibos_tests, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->cola_requerimiento, IPC_RMID, (struct msqid_ds*)0) != -1);
}
