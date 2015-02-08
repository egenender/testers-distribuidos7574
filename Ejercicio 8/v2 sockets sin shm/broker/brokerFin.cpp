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

    key_t key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << SHM_TESTERS_COMUNES_DISPONIBLES << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int shmTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    if (shmTestCom == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << SHM_TESTERS_COMUNES_DISPONIBLES << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    shmctl(shmTestCom, IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << SHM_TESTERS_ESPECIALES_DISPONIBLES << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int shmTestEsp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles), 0660);
    if (shmTestEsp == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << SHM_TESTERS_ESPECIALES_DISPONIBLES << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    shmctl(shmTestEsp, IPC_RMID, NULL);

    Semaphore semTablaCom(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    semTablaCom.getSem();
    semTablaCom.eliSem();
    
    Semaphore semTablaEsp(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    semTablaEsp.getSem();
    semTablaEsp.eliSem();
    
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
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int queue = msgget(key, 0660);
    if (queue == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    msgctl(queue ,IPC_RMID, NULL);
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    queue = msgget(key, 0660);
    if (queue == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    msgctl(queue ,IPC_RMID, NULL);
    
    unlink(ipcFileName.c_str());

    return 0;
}