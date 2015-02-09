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
		
        Semaphore** semTestEspecialesAsignadosEspecificos = new Semaphore* [msg.cantTestersEspecialesAsignados];
        // Busco si los tester especiales están disponibles TODO: Puede flaquear si se desregistra el tester especial en el diome
        semTabla.p();
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            if (tablaTestersEspDisp->disponibles[i]) {
                semTabla.v();
                ss << "Espero a que este disponible el tester especial id " << msg.idTestersEspeciales[i];
                Logger::notice(ss.str(), __FILE__);

                Semaphore semEspecial(msg.idTestersEspeciales[i]);
                semEspecial.getSem();
                semEspecial.p();
                semEspecial.v();
                ss.str("");
                ss << "El tester especial " << msg.idTestersEspeciales[i] << " ya esta disponible";
                Logger::notice(ss.str(), __FILE__);
                semTabla.p();
            }
            // Obtengo el semaforo del tester especial para la asignacion
            semTestEspecialesAsignadosEspecificos[i] = new Semaphore(msg.idTestersEspeciales[i] + SEM_ESPECIALES_ASIGNACION_ESPEC);
        }
        semTabla.v();
        
        ss << "Los testers especiales que se requieren ya estan disponibles";
        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
        
        // ¿Por que se hace esto? Para que los requerimientos especiales lleguen en orden hacia
        // la cantidad total de testers especiales asignados. Basicamente, para que no se 
        // interleaveen cuando dos dispositivos distintos se les asignan los mismos testers 
        // especiales al mismo tiempo. Race condition MUY maldita
        semTestEspAsig.p();
        // Obtengo los semaforos correspondientes
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            semTestEspecialesAsignadosEspecificos[i]->p();
        }
        semTestEspAsig.v();
        // Aca ya tengo todos los testers especiales tomados para mandarlos en orden y que no se 
        // interleaveaee con cualquier otro proceso
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            // Mando cada requerimiento especial especifico
            msg.mtype = msg.idTestersEspeciales[i];
            
            ss.str(""); ss.clear();
            ss << "Se enviará requerimiento especial al tester especial " << msg.mtype;
            Logger::notice(ss.str(), __FILE__);
            
            int ret = msgsnd(msgQueueTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if(ret == -1) {
                Logger::error("Error al enviar mensaje a la msgqueue de testers especiales", __FILE__);
                // Libero los semaforos para que no se cuelgue TODO el sistema
                for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
                    semTestEspecialesAsignadosEspecificos[i]->v();
                    delete semTestEspecialesAsignadosEspecificos[i];
                    delete [] semTestEspecialesAsignadosEspecificos;
                }
                exit(1);
            }
        }
        // Libero los semaforos especificos
        for (int i = 0; i < msg.cantTestersEspecialesAsignados; i++) {
            semTestEspecialesAsignadosEspecificos[i]->v();
            delete semTestEspecialesAsignadosEspecificos[i];
            delete [] semTestEspecialesAsignadosEspecificos;
        }
        exit(0);
	}
}
