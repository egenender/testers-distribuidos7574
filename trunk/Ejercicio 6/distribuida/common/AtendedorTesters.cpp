#include "AtendedorTesters.h"
#include <cstdlib>

AtendedorTesters::AtendedorTesters(): sem_cola_especiales(SEM_COLA_ESPECIALES) {
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
    sem_cola_especiales.getSem();
}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig): sem_cola_especiales(SEM_COLA_ESPECIALES) {
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
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorTesters::recibirResultado(int idTester) {
    //resultado_test_t rsp;
    TMessageAtendedor msg;
    
    int ret = msgrcv(this->cola_recibos_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
        
    return msg.value;

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
	sem_cola_especiales.p();
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
	sem_cola_especiales.v();
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
