/* 
 * File:   terminador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Destruye todos los IPCs utilizados en la aplicacion
 */

#include <cstdlib>
#include "common/common.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "logger/Logger.h"
#include "ipc/Semaphore.h"


int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
   
	//Semaforo y Planilla General
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS,  0660);
    shmctl(shmgeneralid, IPC_RMID, NULL);
    
    Semaphore semPlanillaGeneral(SEM_PLANILLA_GENERAL);
    semPlanillaGeneral.getSem();
    semPlanillaGeneral.eliSem();
    
        //Destruccion de colas
    for (int q = MSGQUEUE_DISPOSITIVOS; q <= MSGQUEUE_DESPACHADOR; q++){
		key = ftok(ipcFileName.c_str(), q);
		int cola = msgget(key, 0660);
		msgctl(cola ,IPC_RMID, NULL);
	}
		
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
		Semaphore sem_tester (SEM_TESTERS_ESPECIALES + i);
		sem_tester.getSem();
		sem_tester.eliSem();
	}
    return 0;
}
