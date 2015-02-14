#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include "../ipc/DistributedSharedMemory.h"

int main (int argc, char* argv[]){
	
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	int msgQueueReqTestEsp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTesterEsp = msgget(key, 0660);

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
    reqMsg.idSubBroker = ID_SUB_BROKER_REQUERIMIENTO_ESP;
        
	TMessageAtendedor msg;
	while(true) {

		Logger::notice("Espero por un requerimiento especial...", __FILE__);
		int okRead = msgrcv(msgQueueReqTestEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if(okRead == -1) {
            Logger::error("Error al leer de la cola de requerimientos especiales", __FILE__);
			exit(0);
		}
		
		std::stringstream ss;
        ss << "Me llega un requerimiento especial del dispositivo " << msg.idDispositivo << " para los testers especiales " << msg.idTestersEspeciales[0] << ", " << msg.idTestersEspeciales[1] << ", " << msg.idTestersEspeciales[2] << ", " << msg.idTestersEspeciales[3];
		Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
		if (fork() != 0) continue;  // Como puedo llegar a bloquearme, lo hago en un proceso aparte!
		
        // Busco si los tester especiales están registrados TODO: Puede flaquear si se desregistra el tester especial en el diome
        
        static int brokersAsignados[MAX_TESTERS_ESPECIALES_PARA_ASIGNAR];
        bool seguiBuscandoTesterEspecial = false;
        do {
            seguiBuscandoTesterEspecial = false;
            
            shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_REQUERIMIENTO_ESP);
            if (shmDistrTablaTesters == NULL) {
                std::stringstream ss;
                ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }

            for (int i = 0; (i < msg.cantTestersEspecialesAsignados) && (!seguiBuscandoTesterEspecial); i++) {
                int id = msg.idTestersEspeciales[i] - ID_TESTER_ESP_START + MAX_TESTER_COMUNES;

                std::stringstream ss; ss << "Busco disponibilidad del tester especial " << msg.idTestersEspeciales[i];
                Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();


                if (!shmDistrTablaTesters->memoria.disponible[id]) {
                    seguiBuscandoTesterEspecial = true;
                    while((--i) >= 0) {
                        shmDistrTablaTesters->memoria.disponible[msg.idTestersEspeciales[i] - ID_TESTER_ESP_START + MAX_TESTER_COMUNES] = true;
                    }
                } else {
                    brokersAsignados[i] = shmDistrTablaTesters->memoria.brokerAsignado[id];
                    shmDistrTablaTesters->memoria.disponible[id] = false;
                }
            }

            shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
            devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
            shmDistrTablaTesters = NULL;

        } while(seguiBuscandoTesterEspecial);

        ss << "Los testers especiales que se requieren ya estan disponibles";
        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
        
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            // Mando cada requerimiento especial especifico
            msg.mtype = msg.idTestersEspeciales[i];
            msg.idBroker = brokersAsignados[i];
            
            ss.str(""); ss.clear();
            ss << "Se enviará requerimiento especial al tester especial " << msg.mtype << " que se encuentra en el broker " << brokersAsignados[i];
            Logger::notice(ss.str(), __FILE__);
            
            if (brokersAsignados[i] == ID_BROKER) {
                int ret = msgsnd(msgQueueTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar mensaje a la msgqueue de testers especiales", __FILE__);
                    exit(1);
                }
            } else {
                // Paso el mensaje hacia otro broker
                msg.tester = msg.idTestersEspeciales[i];
                msg.mtype = brokersAsignados[i];
                msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                int ret = msgsnd(msgQueueTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar mensaje a la msgqueue hacia otros brokers", __FILE__);
                    exit(1);
                }
            }
        }
        exit(0);
	}
}
