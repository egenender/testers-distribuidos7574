#include "AtendedorTesters.h"
#include <cstdlib>

AtendedorTesters::AtendedorTesters() {
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS);
    this->cola_requerimiento = msgget(key, 0666);
    if(this->cola_requerimiento == -1) {
		std::string err = std::string("Error al obtener la cola de requerimientos del atendedor de testers. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS);
    this->cola_recibos_tests = msgget(key, 0666);
    if(this->cola_recibos_tests == -1) {
        std::string err = std::string("Error al obtener la cola de lectura de resultados del atendedor de testers. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
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
        exit(0);
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
        exit(0);
    }

}

resultado_test_t AtendedorTesters::recibirResultado(int idTester) {
    resultado_test_t rsp;
    int ret = msgrcv(this->cola_recibos_tests, &rsp, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
        
    return rsp;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden, int cantidad) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    msg.cant_testers = cantidad;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(int cantidad, int posicion){
	for (int i = 0; i < cantidad; i++){
		posicion_en_shm_t pos;
		pos.mtype = MTYPE_REQUERIMIENTO; //No debe haber tester con id 1
		pos.lugar = posicion;
		int ret = msgsnd(this->cola_recibos_tests, &pos, sizeof(posicion_en_shm_t) - sizeof(long), 0);
		if(ret == -1) {
			std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
			Logger::error(error.c_str(), __FILE__);
			exit(0);
		}
	}
}

int AtendedorTesters::recibirRequerimientoEspecial() {

    posicion_en_shm_t pos;
    int ret = msgrcv(this->cola_recibos_tests, &pos, sizeof(posicion_en_shm_t) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return pos.lugar;
}

bool AtendedorTesters::destruirComunicacion() {

    return (msgctl(this->cola_recibos_tests, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->cola_requerimiento, IPC_RMID, (struct msqid_ds*)0) != -1);
}
