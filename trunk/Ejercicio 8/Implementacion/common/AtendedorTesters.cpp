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
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES);
    this->cola_tareas_especiales = msgget(key, 0666);
    if(this->cola_tareas_especiales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
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
    msg.idDispositivo = idDispositivo;
    msg.idTester = tester;
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

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarTareaEspecial(int idDispositivo, int idTester, int tarea) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = idTester;
    msg.value = tarea;
    int ret = msgsnd(this->cola_tareas_especiales, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar tarea especial " << tarea << " al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(bool cuales[], int idDispositivo){
	sem_cola_especiales.p();
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
            if (!cuales[i]) continue;
            TMessageAssignTE msg;
            msg.mtype = i + ID_TESTER_ESPECIAL_START;
            msg.idDispositivo = idDispositivo;
            std::stringstream ss;
            ss << "Se envia requerimiento especial a tester especial " << msg.mtype;
            Logger::debug(ss.str(), __FILE__);
            ss.str("");
            int ret = msgsnd(this->cola_testers_especiales, &msg, sizeof(TMessageAssignTE) - sizeof(long), 0);
            if(ret == -1) {
                std::string error = std::string("Error al asignar dispositivos a testers especiales. Error: ") + std::string(strerror(errno));
                Logger::error(error.c_str(), __FILE__);
                exit(0);
            }
	}
	sem_cola_especiales.v();
}

int AtendedorTesters::recibirRequerimientoEspecial(int idEsp) {

    TMessageAssignTE msg;
    int ret = msgrcv(this->cola_testers_especiales, &msg, sizeof(TMessageAssignTE) - sizeof(long), idEsp, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;
}

bool AtendedorTesters::destruirComunicacion() {

    return (msgctl(this->cola_recibos_tests, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->cola_requerimiento, IPC_RMID, (struct msqid_ds*)0) != -1);
}
