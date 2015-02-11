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

    // Queue desde donde se reciben mensajes de otros brokers
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM);
	int msgQueueShmemHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM);
	int msgQueueShmemDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_INTERNAL_BROKER_SHM);
	int msgQueueInternaBrokerShmem = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shmCantReqBrokerShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* cantReqBrokerShm = (int*) shmat(shmCantReqBrokerShMem, NULL, 0);

    Semaphore semBrokerCantShmemReq(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semBrokerCantShmemReq.getSem();

    while(true) {
    
        TMessageShMemInterBroker msg;
        // Espero por la llegada de la memoria compartida
        int okRead = msgrcv(msgQueueShmemDesdeBrokers, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0, 0);
        if (okRead == -1) {
            std::stringstream ss;
            ss << "Error al leer de la cola de shared memory desde otros brokers. Errno: " << strerror(errno);
            Logger::error(ss.str(), nombre.str().c_str());
            exit(1);
        }
        
        std::stringstream log;
        log << "Me llego la memoria compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
        /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();
        
        semBrokerCantShmemReq.p();
        int cantRequerimientos = *cantReqBrokerShm;
        semBrokerCantShmemReq.v();
        
        int cantTotalRequerimientos = cantRequerimientos;
        while (cantRequerimientos > 0) {
            log << "Hay " << cantRequerimientos << " modulo(s) del broker " << ID_BROKER << " que necesita(n) la shared memory.";
            Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

            TMessageRequerimientoBrokerShm msgReq;
            okRead = msgrcv(msgQueueInternaBrokerShmem, &msgReq, sizeof(TMessageRequerimientoBrokerShm) - sizeof(long), MTYPE_REQUERIMIENTO_SHM_BROKER, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola interna de gestion de shared memory del broker al buscar requerimiento de shared memory. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            
            log << "El requerimiento obtenido es el del modulo del subbroker " << msgReq.idSubBroker << ". Le envio la memoria...";
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            
            msg.mtype = msgReq.idSubBroker;
            int okSend = msgsnd(msgQueueInternaBrokerShmem, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al enviar shared memory hacia sub-broker " << msg.mtype << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            
            log << "Shared memory enviada al sub-broker " << msg.mtype << ". Espero por su regreso ";
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            
            okRead = msgrcv(msgQueueInternaBrokerShmem, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), MTYPE_DEVOLUCION_SHM_BROKER, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola interna de gestion de shared memory del broker al esperar devolucion de shared memory. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            log << "Shared memory regresa del sub-broker " << msg.mtype;
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            cantRequerimientos--;
        }
        log << "No hay mas requerimientos de shared memory. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
        Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
        
        // Borro los requerimientos saciados recien
        semBrokerCantShmemReq.p();
        *cantReqBrokerShm = *cantReqBrokerShm - cantTotalRequerimientos;
        semBrokerCantShmemReq.v();
        
        if (CANT_BROKERS <= 5) {
            sleep(5 - CANT_BROKERS);
        } else {
            usleep(10000);
        }
        
        // Envio de vuelta la shmem al siguiente broker
        msg.mtype = ID_BROKER_SIGUIENTE;
        int okSend = msgsnd(msgQueueShmemHaciaBrokers, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
        if (okSend == -1) {
            std::stringstream ss;
            ss << "Error al enviar shared memory hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
            Logger::error(ss.str(), nombre.str().c_str());
            exit(1);
        }
    }

    return 0;
}

