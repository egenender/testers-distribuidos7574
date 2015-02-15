/* 
 * File:   brokerShMemPassingHandler.cpp
 * Author: ferno
 *
 * Created on February 11, 2015, 12:37 PM
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/msg.h>
#include <sys/shm.h>

#include "../logger/Logger.h"
#include "../common/common.h"
#include "../ipc/Semaphore.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << ID_BROKER;

    // Queues contra brokers (envio y recepcion de shmems)
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_GENERAL);
	int msgQueuePlanillaGeneralHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL);
	int msgQueuePlanillaGeneralDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_ASIGNACION);
	int msgQueuePlanillaAsignacionHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION);
	int msgQueuePlanillaAsignacionDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    // Queues contra testers/equipo (envio y recepcion de shmems)
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER);
	int msgQueuePlanillasHaciaTesters = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER);
	int msgQueuePlanillasDesdeTesters = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER);
	int msgQueueCantReqShmemPlanillas = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    int shmCantReqPlanillasShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    TShmemCantRequerimientos* cantReqPlanillasShm = (TShmemCantRequerimientos*) shmat(shmCantReqPlanillasShMem, NULL, 0);

    Semaphore semBrokerCantShmemReq(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semBrokerCantShmemReq.getSem();
    
    if(fork() == 0) {
        // Proceso encargado de la shmem Planilla General
        while(true) {
            TSharedMemoryPlanillaGeneral msg;
            // Espero por la llegada de la memoria compartida desde otros brokers
            int okRead = msgrcv(msgQueuePlanillaGeneralDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola de shared memory de planilla general desde otros brokers. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }

            std::stringstream log;
            log << "Me llego la planilla general compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
            /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();

            semBrokerCantShmemReq.p();
            int cantRequerimientos = cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral;
            semBrokerCantShmemReq.v();

            int cantTotalRequerimientos = cantRequerimientos;
            while (cantRequerimientos > 0) {
                log << "Hay " << cantRequerimientos << " testers/equipo comunicados con broker " << ID_BROKER << " que necesita(n) la planilla general.";
                Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                TRequerimientoSharedMemory msgReq;
                okRead = msgrcv(msgQueueCantReqShmemPlanillas, &msgReq, sizeof(TRequerimientoSharedMemory) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_GENERAL, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola interna de requerimientos de planillas. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "El requerimiento obtenido es el del tester/equipo " << msgReq.idSolicitante << ". Le envio la planilla general...";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                msg.mtype = msgReq.idDevolucion;
                int okSend = msgsnd(msgQueuePlanillasHaciaTesters, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia tester/equipo " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "Planilla general enviada al tester/equipo " << msg.mtype << ". Espero por su regreso ";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                okRead = msgrcv(msgQueuePlanillasDesdeTesters, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola de devolucion de planilla general. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                log << "Planilla general regresa";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
                cantRequerimientos--;
            }
            log << "No hay mas requerimientos de planilla general. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

            // Borro los requerimientos saciados recien
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral = cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral - cantTotalRequerimientos;
            semBrokerCantShmemReq.v();

            // Pequeño sleep para no matar al CPU si hay pocos brokers
            if (CANT_BROKERS <= 3) {
                sleep(3 - CANT_BROKERS);
            } else {
                usleep(10000);
            }

            if (CANT_BROKERS == 1) {
                // Soy el unico. Me la envio a mi mismo
                msg.mtype = ID_BROKER;
                int okSend = msgsnd(msgQueuePlanillaGeneralDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia mi. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                } 
            } else {
                // Envio de vuelta la shmem al siguiente broker
                msg.mtype = ID_BROKER_SIGUIENTE;
                int okSend = msgsnd(msgQueuePlanillaGeneralHaciaBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
            }
        }
    }
    
    if(fork() == 0) {
        // Proceso encargado de la shmem Planilla Asignacion
        while(true) {
            TSharedMemoryPlanillaAsignacion msg;
            // Espero por la llegada de la memoria compartida desde otros brokers
            int okRead = msgrcv(msgQueuePlanillaAsignacionDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola de planilla asignacion desde otros brokers. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }

            std::stringstream log;
            log << "Me llego la planilla asignacion compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
            /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();

            semBrokerCantShmemReq.p();
            int cantRequerimientos = cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion;
            semBrokerCantShmemReq.v();

            int cantTotalRequerimientos = cantRequerimientos;
            while (cantRequerimientos > 0) {
                log << "Hay " << cantRequerimientos << " testers/equipo comunicados con broker " << ID_BROKER << " que necesita(n) la planilla asignacion.";
                Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                TRequerimientoSharedMemory msgReq;
                okRead = msgrcv(msgQueueCantReqShmemPlanillas, &msgReq, sizeof(TRequerimientoSharedMemory) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola interna de requerimientos de planillas. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "El requerimiento obtenido es el del tester/equipo " << msgReq.idSolicitante << ". Le envio la planilla asignacion...";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                msg.mtype = msgReq.idSolicitante;
                int okSend = msgsnd(msgQueuePlanillasHaciaTesters, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia tester/equipo " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "Planilla asignacion enviada al tester/equipo " << msg.mtype << ". Espero por su regreso ";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                okRead = msgrcv(msgQueuePlanillasDesdeTesters, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola de devolucion de planilla asignacion. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                log << "Planilla asignacion regresa";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
                cantRequerimientos--;
            }
            log << "No hay mas requerimientos de planilla asignacion. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

            // Borro los requerimientos saciados recien
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion = cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion - cantTotalRequerimientos;
            semBrokerCantShmemReq.v();

            usleep(100000); // sleepeo un toque para no agitar al procesador
            if (CANT_BROKERS == 1) {
                // Soy el unico. Me la envio a mi mismo
                msg.mtype = ID_BROKER;
                int okSend = msgsnd(msgQueuePlanillaAsignacionDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia mi. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                } 
            } else {
                // Envio de vuelta la shmem al siguiente broker
                msg.mtype = ID_BROKER_SIGUIENTE;
                int okSend = msgsnd(msgQueuePlanillaAsignacionHaciaBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
            }
        }
    }

    return 0;
}

