#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "ipc/DistributedSharedMemory.h"

int main (void) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	int msgQueueReqDisp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTesterComun = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueADisp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_INTERNAL_BROKER_SHM);
	int msgQueueShm = msgget(key, 0660);
    
    // Queue hacia donde se envian mensajes de otros brokers
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_HACIA_BROKER);
	int msgQueueHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shMemCantReqBrokerShmemId = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* shMemCantReqBrokerShmem = (int*) shmat(shMemCantReqBrokerShmemId, (void*) NULL, 0);
    
    Semaphore semCantReqBrokerShmem(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semCantReqBrokerShmem.getSem();

    TMessageShMemInterBroker* shmDistrTablaTesters = NULL;
    TMessageRequerimientoBrokerShm reqMsg;
    reqMsg.mtype = MTYPE_REQUERIMIENTO_SHM_BROKER;
    reqMsg.idSubBroker = ID_SUB_BROKER_REGISTRO_TESTER;

	TMessageAtendedor msg;
	while(true) {
		Logger::notice("Espero por un requerimiento de dispositivo...", __FILE__);
		int okRead = msgrcv(msgQueueReqDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
			exit(0);
		}

		std::stringstream ss;
		ss << "Me llego un requerimiento desde el dispositivo " << msg.idDispositivo;
		Logger::notice(ss.str(), __FILE__);

        bool exito = true;

        shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_REQUERIMIENTO_DISP);
        if (shmDistrTablaTesters == NULL) {
            std::stringstream ss;
            ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        // Busco un id que este registrado
        for(int i = shmDistrTablaTesters->memoria.ultimoTesterElegido + 1; i <= MAX_TESTER_COMUNES; i++) {
            if(i == MAX_TESTER_COMUNES) i = 0;
            if(shmDistrTablaTesters->memoria.registrados[i]) {
                msg.mtype = i + ID_TESTER_COMUN_START;
                msg.idBroker = shmDistrTablaTesters->memoria.brokerAsignado[i];
                shmDistrTablaTesters->memoria.ultimoTesterElegido = i;
                break;
            }
            if (i == shmDistrTablaTesters->memoria.ultimoTesterElegido) {
                std::stringstream ss;
                ss << "No hay tester comun disponible para el dispositivo " << msg.idDispositivo;
                Logger::error(ss.str(), __FILE__);
                exito = false;
                break;
            }
        }
        shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
        devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
        shmDistrTablaTesters = NULL;

        ss.str("");
        ss.clear();
        // ¿Puede ser que no haya testers comunes registrados? En ese caso
        // le envio msg al dispositivo con el "SIN_LUGAR"
        if (!exito) {
            ss << "Le envio que no hay lugar al dispositivo " << msg.idDispositivo;
            Logger::notice(ss.str(), __FILE__);
            
            msg.mtype = msg.idDispositivo;
            msg.value = SIN_LUGAR;
            int ret = msgsnd(msgQueueADisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if(ret == -1) {
                Logger::error("Error al enviar el SIN_LUGAR al dispositivo", __FILE__);
                exit(1);
            }
        } else {
            ss << "Le envio el requerimiento del dispositivo al Tester " << msg.mtype << " que se encuentra en el broker " << msg.idBroker;
            Logger::notice(ss.str(), __FILE__);

            if (msg.idBroker == ID_BROKER) {
                int ret = msgsnd(msgQueueTesterComun, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el requerimiento del dispositivo al tester", __FILE__);
                    exit(1);
                }
            } else {
                // Paso el mensaje al broker correspondiente
                msg.mtype = msg.idBroker;
                msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                int ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el requerimiento del dispositivo al tester", __FILE__);
                    exit(1);
                }
            }
        }
	}
}