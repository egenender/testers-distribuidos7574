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

    key_t key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int shmCantReqShmem = shmget(key, sizeof(int), 0660);
    if (shmCantReqShmem == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    shmctl(shmCantReqShmem, IPC_RMID, NULL);

    Semaphore semTablaCom(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semTablaCom.getSem();
    semTablaCom.eliSem();
    
    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    if (key == -1) {
        std::stringstream ss;
        ss << "Error obteniendo la key del IPC " << SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    int shmCantReqPlanillas = shmget(key, sizeof(int), 0660);
    if (shmCantReqPlanillas == -1) {
        std::stringstream ss;
        ss << "Error obteniendo el ID del IPC " << SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM << ". Error: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    shmctl(shmCantReqPlanillas, IPC_RMID, NULL);

    Semaphore semReqPlanillas(SEM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    semReqPlanillas.getSem();
    semReqPlanillas.eliSem();

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

    for (int q = MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS; q <= MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER; q++) {
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