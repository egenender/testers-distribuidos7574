#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include "common/common.h"
#include "logger/Logger.h"
#include "ipc/Semaphore.h"
#include "ipc/DistributedSharedMemory.h"

int main(int argc, char* argv[]) {

	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTestYEq = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	int msgQueueReqDisp = msgget(key, 0660);
    
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
    int brokerEquipoEspecial = -1;
	
	TMessageAtendedor msg;
    int ret = 0;
    std::stringstream ss;
	while(true) {
		Logger::notice("Espero un mensaje de un dispositivo", __FILE__);
		int okRead = msgrcv(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
            Logger::error("Error al recibir un mensaje de la cola del dispositivo", __FILE__);
			exit(1);
		}
        
        switch(msg.mtypeMensaje) {
        
            case MTYPE_REQUERIMIENTO_DISPOSITIVO:
                ss << "Llego un requerimiento del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ret = msgsnd(msgQueueReqDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de requerimientos de dispositivo");
                    exit(1);
                }
                break;
            
            case MTYPE_RESULTADO_INICIAL:
                ss << "Llego resultado de test inicial del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ss << "El resultado inicial debe ser enviado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                
                if (msg.idBroker == ID_BROKER) {
                    // El mensaje es de para tester en este broker
                    msg.mtype = msg.tester;
                    ret = msgsnd(msgQueueTestYEq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a testers comunes", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío resultado inicial hacia el broker " << msg.idBroker;
                    Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // El mensaje es para tester en otro broker
                    msg.mtype = msg.idBroker;
                    msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio hacia brokers", __FILE__);
                        exit(1);
                    }
                }
                break;
/* NOTE: Lo necesitare para el anillo
            case MTYPE_REQ_DISP_DESDE_BROKER:
                std::stringstream ss;
                ss << "Llego requerimiento desde otro broker del disp " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ss << "Este requerimiento debe ser enviado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                
                if (msg.idBroker == ID_BROKER) {
                    // El mensaje es de para tester en este broker
                    msg.mtype = msg.tester;
                    ret = msgsnd(msgQueueTestYEq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a testers comunes", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío resultado inicial hacia el broker " << msg.idBroker;
                    Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // El mensaje es para tester en otro broker
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio hacia brokers", __FILE__);
                        exit(1);
                    }
                }
                break;
*/
            case MTYPE_RESULTADO_ESPECIAL:
                std::stringstream ss;
                ss << "Llego resultado de test especial del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                if(fork() == 0) {
                    // Tengo que ver en que broker está el tester especial (si es que ya no lo guarde)
                    if(brokerEquipoEspecial == -1) {
                        // Busco el broker donde se registro el equipo especial
                        // Pido la shmem y vuelvo a poner al tester disponible
                        shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_PASAMANOS_RECEPTOR);
                        if (shmDistrTablaTesters == NULL) {
                            std::stringstream ss;
                            ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
                            Logger::error(ss.str(), __FILE__);
                            exit(1);
                        }
                        
                        brokerEquipoEspecial = shmDistrTablaTesters->memoria.brokerAsignado[ID_EQUIPO_ESPECIAL - ID_TESTER_ESP_START + MAX_TESTER_COMUNES];
                        ss << "El broker donde esta registrado el equipo especial es el broker " << brokerEquipoEspecial;
                        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();

                        shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
                        devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
                        shmDistrTablaTesters = NULL;
                    }
                    
                    msg.idBroker = brokerEquipoEspecial;

                    ss << "El resultado especial debe ser enviado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                    Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();

                    if (msg.idBroker == ID_BROKER) {
                        msg.mtype = ID_EQUIPO_ESPECIAL;
                        int ret = msgsnd(msgQueueTestYEq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                        if(ret == -1) {
                            Logger::error("Error al enviar el resultado especial a la cola de envio a testers y equipo especial", __FILE__);
                            exit(1);
                        }
                    } else {
                        ss << "Envío resultado especial hacia el broker " << msg.idBroker;
                        Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                        // El mensaje es para tester en otro broker
                        msg.mtype = msg.idBroker;
                        msg.mtypeMensajeBroker = MTYPE_HACIA_EQUIPO_ESPECIAL;
                        ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                        if(ret == -1) {
                            Logger::error("Error al enviar el resultado especial a la cola de envio hacia brokers", __FILE__);
                            exit(1);
                        }
                    }
                }
                break;
        }
	}
    return 0;
}