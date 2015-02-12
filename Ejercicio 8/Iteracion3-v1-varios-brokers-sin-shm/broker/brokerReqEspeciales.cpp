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

    Semaphore semTestEspAsig(SEM_ESPECIALES_ASIGNACION);
    semTestEspAsig.getSem();
        
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
        shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_REQUERIMIENTO_ESP);
        if (shmDistrTablaTesters == NULL) {
            std::stringstream ss;
            ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        int brokersAsignados[MAX_TESTERS_ESPECIALES_PARA_ASIGNAR];
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            if (!shmDistrTablaTesters->memoria.registrados[msg.idTestersEspeciales[i] - ID_TESTER_ESP_START + MAX_TESTER_COMUNES]) { // Le sumo el MAX_TESTER_COMUNES porque es una tabla de tamanio MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES
                shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
                devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
                shmDistrTablaTesters = NULL;

                ss << "Espero a que este disponible el tester especial id " << msg.idTestersEspeciales[i];
                Logger::notice(ss.str(), __FILE__);

                Semaphore semEspecial(msg.idTestersEspeciales[i] - ID_TESTER_ESP_START + SEM_ESPECIALES);
                semEspecial.getSem();
                semEspecial.p();
                semEspecial.v();
                ss.str("");
                ss << "El tester especial " << msg.idTestersEspeciales[i] << " ya esta disponible";
                Logger::notice(ss.str(), __FILE__);
                
                shmDistrTablaTesters = (TMessageShMemInterBroker*) obtenerDistributedSharedMemory(msgQueueShm, &reqMsg, sizeof(TMessageRequerimientoBrokerShm), shMemCantReqBrokerShmem, &semCantReqBrokerShmem, msgQueueShm, sizeof(TMessageShMemInterBroker), ID_SUB_BROKER_REQUERIMIENTO_ESP);
                if (shmDistrTablaTesters == NULL) {
                    std::stringstream ss;
                    ss << "Error intentando obtener la memoria compartida distribuida para el broker " << ID_BROKER << " y sub-broker " << ID_SUB_BROKER_REGISTRO_TESTER << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), __FILE__);
                    exit(1);
                }
            }
            brokersAsignados[i] = shmDistrTablaTesters->memoria.brokerAsignado[msg.idTestersEspeciales[i] - ID_TESTER_ESP_START + MAX_TESTER_COMUNES];
        }
        shmDistrTablaTesters->mtype = MTYPE_DEVOLUCION_SHM_BROKER;
        devolverDistributedSharedMemory(msgQueueShm, shmDistrTablaTesters, sizeof(TMessageShMemInterBroker));
        shmDistrTablaTesters = NULL;
        
        ss << "Los testers especiales que se requieren ya estan disponibles";
        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
        
        // ¿Por que se toma este semaforo? Para que los requerimientos especiales lleguen en orden a
        // la cantidad total de testers especiales asignados. Basicamente, para que no se 
        // interleaveen cuando dos dispositivos distintos se les asignan los mismos testers 
        // especiales al mismo tiempo. Race condition MUY maldita
        semTestEspAsig.p();
        // Aca ya tengo todos los testers especiales tomados para mandarlos en orden y que no se 
        // interleaveaee con cualquier otro proceso
        // TODO: AL DISTRIBUIR LOS ESPECIALES, INTUYO QUE SE IRA TODO A LA GOMA, 
        // PORQUE NO PODEMOS APELAR A QUE LOS TESTERS ESPECIALES DESPERDIGADOS
        // POR TODOS LADOS LES LLEGUEN DE TODOS LADOS EN ORDEN. KB O QQQ
        // SOLUCION: NO DISTRIBUIR TESTERS ESPECIALES, O QUE EN LA SHMEM SALGA
        // SI ESTA EN USO O NO Y LUEGO VER COMO LO SETEAMOS
        // TAMBIEN VER SI LA ELECCION DE QUE TESTER ESPECIAL PASA AL BROKER
        // EN VEZ DE QUE LA TENGA EL MAS ALTO NIVEL (TESTERCOMUN.CPP)
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
                    semTestEspAsig.v();
                    exit(1);
                }
            } else {
                // Paso el mensaje hacia otro broker
                msg.tester = msg.idTestersEspeciales[i];
                msg.mtype = brokersAsignados[i];
                msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                int ret = msgsnd(msgQueueTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar mensaje a la msgqueue de testers especiales", __FILE__);
                    semTestEspAsig.v();
                    exit(1);
                }
            }
        }
        semTestEspAsig.v();
        exit(0);
	}
}
