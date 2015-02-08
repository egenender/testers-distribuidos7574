#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"


int main (int argc, char* argv[]){
	
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	int msgQueueReqTestEsp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTesterEsp = msgget(key, 0660);

	key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    int shmTablaTestersEspDisp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles), 0660);
    TTablaIdTestersEspecialesDisponibles* tablaTestersEspDisp = (TTablaIdTestersEspecialesDisponibles*) shmat(shmTablaTestersEspDisp, (void*) NULL, 0);
    
    Semaphore semTabla(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    semTabla.getSem();
        
	TMessageAtendedor msg;
	while(true) {

		Logger::notice("Espero por un requerimiento especial...", __FILE__);
		int okRead = msgrcv(msgQueueReqTestEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if(okRead == -1) {
            Logger::error("Error al leer de la cola de requerimientos especiales", __FILE__);
			exit(0);
		}
		
		std::stringstream ss;
		Logger::notice("Me llega un requerimiento especial...", __FILE__);
		if (fork() != 0) continue;  // Como puedo llegar a bloquearme, lo hago en un proceso aparte!
		
        // Busco si el tester especial está disponible
        semTabla.p();
        if (tablaTestersEspDisp->disponibles[msg.tester - ID_TESTER_ESP_START]) {
            semTabla.v();
            ss << "Espero a que este disponible el tester especial id " << msg.tester - ID_TESTER_ESP_START;
            Logger::notice(ss.str(), __FILE__);
            
            Semaphore semEspecial(msg.tester);
            semEspecial.getSem();
            semEspecial.p();
            ss.str("");
            ss << "El tester especial " << msg.tester << " ya esta disponible";
            Logger::notice(ss.str(), __FILE__);
        }
        semTabla.v();

        msg.mtype = msg.tester;
        ss.str(""); ss.clear();
        ss << "Se enviará requerimiento especial al tester especial " << msg.mtype;
        Logger::notice(ss.str(), __FILE__);
        int ret = msgsnd(msgQueueTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
        if(ret == -1) {
            Logger::error("Error al enviar mensaje a la msgqueue de testers especiales", __FILE__);
            exit(1);
        }

        exit(0);
	}
}
