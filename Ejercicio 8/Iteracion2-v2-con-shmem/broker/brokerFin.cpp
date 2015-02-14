/* 
 * File:   brokerFin.cpp
 * Author: ferno
 *
 */

#include <cstdlib>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

#include "common/common.h"
#include "../ipc/Semaphore.h"
#include "logger/Logger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    key_t key = ftok(ipcFileName.c_str(), SHM_BROKER_TESTERS_REGISTRADOS);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << SHM_BROKER_TESTERS_REGISTRADOS << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int shmTestCom = shmget(key, sizeof(TTablaBrokerTestersRegistrados), 0660);
    if (shmTestCom == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << SHM_BROKER_TESTERS_REGISTRADOS << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    shmctl(shmTestCom, IPC_RMID, NULL);

    Semaphore semTablaCom(SEM_BROKER_TESTERS_REGISTRADOS);
    semTablaCom.getSem();
    semTablaCom.eliSem();
    
    Semaphore semEspecialAsignacion(SEM_ESPECIALES_ASIGNACION);
    semEspecialAsignacion.getSem();
    semEspecialAsignacion.eliSem();
    
    for (int i = 0; i < MAX_TESTER_ESPECIALES; i++) {
		Semaphore semEspecial(i + SEM_ESPECIALES);
		semEspecial.getSem();
		semEspecial.eliSem();
	}
    
    //Destruccion de colas
    for (int q = MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS; q <= MSGQUEUE_BROKER_EMISOR; q++) {
        key = ftok(ipcFileName.c_str(), q);
        if (key == -1) {
            std::stringstream ss;
            ss << "Error obteniendo la key del IPC " << q << ". Error: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        int cola = msgget(key, 0660);
        if (cola == -1) {
            std::stringstream ss;
            ss << "Error obteniendo el ID del IPC " << q << ". Error: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        msgctl(cola ,IPC_RMID, NULL);
    }

    for (int q = MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER; q <= MSGQUEUE_BROKER_REGISTRO_TESTERS; q++) {
        key = ftok(ipcFileName.c_str(), q);
        if (key == -1) {
            std::stringstream ss;
            ss << "Error obteniendo la key del IPC " << q << ". Error: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        int cola = msgget(key, 0660);
        if (cola == -1) {
            std::stringstream ss;
            ss << "Error obteniendo el ID del IPC " << q << ". Error: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        msgctl(cola ,IPC_RMID, NULL);
    }
    
    unlink(ipcFileName.c_str());

    return 0;
}