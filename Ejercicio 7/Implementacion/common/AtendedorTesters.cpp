#include "AtendedorTesters.h"
#include <cstdlib>
#include <string>
#include "../logger/Logger.h"

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
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_ESPECIALES);
    this->cola_testers_especiales = msgget(key, 0666);
    if(this->cola_testers_especiales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar a los testers especiales. Errno: ") + std::string(strerror(errno));
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
    msg.tester = tester; 
    msg.value = idPrograma;
    msg.idDispositivo = 0;
    msg.cant_testers = 0;
 
	std::stringstream ss;
	ss << "Envio programa a dispositivo " << idDispositivo << " desde el tester " << tester;
	Logger::notice(ss.str(), __FILE__);
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

void AtendedorTesters::enviarAEspeciales(bool cuales[], int posicion){
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
		posicion_en_shm_t pos;
		pos.mtype = i + ID_TESTER_ESPECIAL_START ;
		pos.lugar = posicion;
		if (!cuales[i]) continue;
		int ret = msgsnd(this->cola_testers_especiales, &pos, sizeof(posicion_en_shm_t) - sizeof(long), 0);
		if(ret == -1) {
			std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
			Logger::error(error.c_str(), __FILE__);
			exit(0);
		}
	}
}

int AtendedorTesters::recibirRequerimientoEspecial(int idEsp) {

    posicion_en_shm_t pos;
    int ret = msgrcv(this->cola_testers_especiales, &pos, sizeof(posicion_en_shm_t) - sizeof(long), idEsp, 0);
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
