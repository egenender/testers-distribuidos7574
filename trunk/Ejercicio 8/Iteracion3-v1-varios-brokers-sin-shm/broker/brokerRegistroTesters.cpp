/* 
 * File:   brokerRegistroTesters.cpp
 * Author: ferno
 *
 * Created on February 9, 2015, 10:44 PM
 */

#include <cstdlib>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstring>
#include <cerrno>

#include "logger/Logger.h"
#include "ipc/Semaphore.h"
#include "ipc/DistributedSharedMemory.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
	int msgQueueRegistros = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_INTERNAL_BROKER_SHM);
	int msgQueueShm = msgget(key, 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shMemCantReqBrokerShmemId = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* shMemCantReqBrokerShmem = (int*) shmat(shMemCantReqBrokerShmemId, (void*) NULL, 0);
    
    Semaphore semCantReqBrokerShmem(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semCantReqBrokerShmem.getSem();

    TMessageShMemInterBroker* shmDistrTablaTesters = NULL;
    TMessageRequerimientoBrokerShm reqMsg;
    reqMsg.mtype = MTYPE_REQUERIMIENTO_SHM_BROKER;
    reqMsg.idSubBroker = ID_SUB_BROKER_REGISTRO_TESTER;
    
    while(true) {

        TMessageAtendedor msg;
        Logger::notice("Espero por un pedido de registro...", __FILE__);
		int okRead = msgrcv(msgQueueRegistros, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
			exit(0);
		}

		std::stringstream ss;
		ss << "Me llego un pedido de requerimiento desde el tester " << msg.tester;
		Logger::notice(ss.str(), __FILE__);
        
        // Calculo el ID en la tabla de testers registrados en base a si es 
        // Tester Comun -> Al ppio de la tabla; Tester Especial -> al final de la tabla
        int id = msg.tester;
        if (msg.esTesterEspecial) {
            id = id - ID_TESTER_ESP_START + MAX_TESTER_COMUNES;
        } else  id -= ID_TESTER_COMUN_START;
        
        shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_REGISTRO_TESTER);
        if (shmDistrTablaTesters == NULL) {
            std::stringstream ss;
            ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        if (!shmDistrTablaTesters->memoria.registrados[id]) {
            shmDistrTablaTesters->memoria.registrados[id] = true;
            shmDistrTablaTesters->memoria.brokerAsignado[id] = ID_BROKER;

            if (msg.esTesterEspecial) {
                // Levanto el semaforo por si hay un requerimiento especial esperando
                Semaphore semEspecial(SEM_ESPECIALES + id - MAX_TESTER_COMUNES);
                semEspecial.getSem();
                semEspecial.v();
            }
            
            std::stringstream ss;
            ss << "Se ha registrado al tester " << msg.tester << " con exito";
            Logger::notice(ss.str(), __FILE__);

        } else {
            std::stringstream ss;
            ss << "El tester " << msg.tester << " ya se encontraba registrado en el sistema!";
            Logger::error(ss.str(), __FILE__);
            // TODO: ¿Que hacemos aca?
        }
        shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
        devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
        shmDistrTablaTesters = NULL;
    }

    return 0;
}

