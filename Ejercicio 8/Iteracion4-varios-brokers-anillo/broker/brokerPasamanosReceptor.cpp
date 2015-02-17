/* 
 * File:   brokerPasamanosReceptor.cpp
 * Author: ferno
 *
 * Created on February 6, 2015, 1:43 AM
 */

#include <cstdlib>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "common/common.h"
#include "logger/Logger.h"
#include "ipc/Semaphore.h"
#include "ipc/DistributedSharedMemory.h"

using namespace std;

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	int msgQueueReceptor = msgget(key, 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueEmisor = msgget(key, 0660);

	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);

	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	int msgQueueReqTestEsp = msgget(key, 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
    int msgQueueRegistroTesters = msgget(key, 0660);

    // Queue hacia donde se envian mensajes de otros brokers
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_HACIA_BROKER);
	int msgQueueHaciaBrokers = msgget(key, IPC_CREAT | 0660);
    
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
    reqMsg.idSubBroker = ID_SUB_BROKER_PASAMANOS_RECEPTOR;

	TMessageAtendedor msg;
    int ret = 0;
    std::stringstream ss;
	while(true) {
		Logger::notice("Espero un mensaje de un tester", __FILE__);
		int okRead = msgrcv(msgQueueReceptor, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
            Logger::error("Error al recibir un mensaje de la cola de los testers", __FILE__);
			exit(1);
		}

        switch(msg.mtypeMensaje) {

            case MTYPE_REQUERIMIENTO_TESTER_ESPECIAL:
                ss << "Llego un requerimiento del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                ret = msgsnd(msgQueueReqTestEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de requerimientos de dispositivo");
                    exit(1);
                }
                break;

            case MTYPE_REGISTRAR_TESTER:
                ss << "Llego un pedido de registro del tester " << msg.tester;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                ret = msgsnd(msgQueueRegistroTesters, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de registros de testers", __FILE__);
                    exit(1);
                }
                break;

            case MTYPE_DESREGISTRAR_TESTER:
                ss << "Llego un pedido de desregistro del tester " << msg.tester;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                ret = msgsnd(msgQueueRegistroTesters, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de registros/desregistros de testers", __FILE__);
                    exit(1);
                }
                break;

            case MTYPE_HAY_QUE_REINICIAR:
                ss << "Llego un mensaje para reiniciar (o no) para el tester especial " << msg.tester;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                // En un proceso aparte, consulto que broker es este tester y envio
                if (fork() == 0) {
                    // Pido la shmem y me fijo el broker en el que esta registrado
                    shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_PASAMANOS_RECEPTOR);
                    if (shmDistrTablaTesters == NULL) {
                        std::stringstream ss;
                        ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
                        Logger::error(ss.str(), __FILE__);
                        exit(1);
                    }
                    
                    int brokerAsignado = shmDistrTablaTesters->memoria.brokerAsignado[msg.tester - ID_TESTER_ESP_START + MAX_TESTER_COMUNES] = true;
                    
                    shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
                    devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
                    shmDistrTablaTesters = NULL;
                    
                    ss << "El tester especial esta registrado en el broker " << brokerAsignado << " y yo estoy en el broker " << ID_BROKER;
                    Logger::notice(ss.str(), __FILE__);
                    ss.str("");
                    ss.clear();
                    
                    msg.idBroker = brokerAsignado;
                    // Envio al broker correspondiente
                    if(brokerAsignado == ID_BROKER) {
                        msg.mtype = msg.tester;
                        ret = msgsnd(msgQueueEmisor, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                        if(ret == -1) {
                            Logger::error("Error al enviar el mensaje a la cola de envio a testers", __FILE__);
                            exit(1);
                        }
                    } else {
                        ss << "Envío el aviso de reinicio al broker correspondiente de ID " << msg.idBroker;
                        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
                        // Envio el mensaje al broker correspondiente
                        msg.mtype = msg.idBroker;
                        msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                        ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                        if(ret == -1) {
                            Logger::error("Error al enviar el mensaje a la cola de envio a otros brokers", __FILE__);
                            exit(1);
                        }
                    }
                    
                    exit(0);
                }
                break;

            case MTYPE_AVISAR_DISPONIBILIDAD:
                ss << "Llego un aviso de disponibilidad del tester especial " << msg.tester;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                
                if (fork() == 0) {
                    // Pido la shmem y vuelvo a poner al tester disponible
                    shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_PASAMANOS_RECEPTOR);
                    if (shmDistrTablaTesters == NULL) {
                        std::stringstream ss;
                        ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
                        Logger::error(ss.str(), __FILE__);
                        exit(1);
                    }
                    
                    shmDistrTablaTesters->memoria.disponible[msg.tester - ID_TESTER_ESP_START + MAX_TESTER_COMUNES] = true;
                    
                    shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
                    devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
                    shmDistrTablaTesters = NULL;
                    
                    exit(0);
                }
                break;

            default:
                ss << "Llego mensaje para el dispositivo " << msg.idDispositivo << " que esta conectado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                if (msg.idBroker == ID_BROKER) {
                    msg.mtype = msg.idDispositivo;
                    ret = msgsnd(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a dispositivos", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío el mensaje al broker correspondiente de ID " << msg.idBroker;
                    Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // Envio el mensaje al broker correspondiente
                    msg.mtype = msg.idBroker;
                    msg.mtypeMensajeBroker = MTYPE_HACIA_DISPOSITIVO;
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a otros brokers", __FILE__);
                        exit(1);
                    }
                }
                break;
        }
	}
    return 0;
}