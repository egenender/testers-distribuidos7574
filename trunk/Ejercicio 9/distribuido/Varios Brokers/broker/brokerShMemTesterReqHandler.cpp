/* 
 * File:   brokerShMemTesterReqHandler.cpp
 * Author: ferno
 *
 * Created on February 14, 2015, 8:31 PM
 * 
 * Basicamente, se encarga de contabilizar los requerimientos de memoria compartida de los 
 * testers y equipo especial. Lee de la msgqueue de requerimientos externa (donde se guardan 
 * los requerimientos que vienen de los testers), contabiliza en la shared memory y deposita 
 * el requerimiento en la msqueue de requerimientos interna
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/msg.h>
#include <sys/shm.h>

#include "logger/Logger.h"
#include "common/common.h"
#include "ipc/Semaphore.h"
#include "sync/Timeout.h"

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER);
	int msgQueueReqInterna = msgget(key, IPC_CREAT | 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTO_SHMEM_HANDLER);
	int msgQueueReqExterna = msgget(key, IPC_CREAT | 0660);
    
    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    int shmCantReqPlanillasShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    TShmemCantRequerimientos* cantReqPlanillasShm = (TShmemCantRequerimientos*) shmat(shmCantReqPlanillasShMem, NULL, 0);

    Semaphore semBrokerCantShmemReq(SEM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    semBrokerCantShmemReq.getSem();
    
    while(true) {
    
        TRequerimientoSharedMemory req;
        
        // Espero por un requerimiento de memoria compartida de los testers
        int okRead = msgrcv(msgQueueReqExterna, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0, 0);
        if (okRead == -1) {
            std::stringstream ss;
            ss << "Error al leer de la cola de requerimientos externa. Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        
        // Sumo el requerimiento en la shmem y luego lo inserto en la msgqueue
        if(req.mtype == MTYPE_REQ_SHMEM_PLANILLA_GENERAL) {
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral += 1;
            semBrokerCantShmemReq.v();
        } else if(req.mtype == MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION) {
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion += 1;
            semBrokerCantShmemReq.v();
        }
        
        int okSend = msgsnd(msgQueueReqInterna, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
        if (okSend == -1) {
            std::stringstream ss;
            ss << "Error al enviar requerimiento de tester/equipo " << req.idSolicitante << " a la msgqueue interna de requerimientos. Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
    }

    return 0;
}

