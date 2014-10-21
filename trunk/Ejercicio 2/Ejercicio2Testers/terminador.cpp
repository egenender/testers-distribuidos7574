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
#include "common/planilla_local.h"

/*
 * 
 */

void createIPCObjects();
void createSystemProcesses();

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
   
	//Semaforo y Planilla General
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof(int), IPC_CREAT | 0660);
    shmctl(shmgeneralid, IPC_RMID, NULL);
    
    Semaphore semPlanillaGeneral(SEM_PLANILLA_GENERAL);
    semPlanillaGeneral.getSem();
    semPlanillaGeneral.eliSem();
    
    //Semaforos de Tester A y B, y planillas Locales
    for (int i = 0; i < CANT_TESTERS; i++){
        Semaphore semtesterA(SEM_TESTER_A + i);
        Semaphore semtesterB(SEM_TESTER_B + i);
        Semaphore semlocal(SEM_PLANILLA_LOCAL + i);
        
        semtesterA.getSem();
        semtesterA.eliSem();
        semtesterB.getSem();
        semtesterB.eliSem();
        semlocal.getSem();
        semlocal.eliSem();
        
        key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + i);
        int shmlocalid = shmget(key, sizeof(planilla_local_t), IPC_CREAT | 0660);
        shmctl(shmlocalid, IPC_RMID, NULL);
    }
    
    //Destruccion de colas
    for (int q = MSGQUEUE_PLANILLA; q <= MSGQUEUE_DESPACHADOR; q++){
		key = ftok(ipcFileName.c_str(), q);
		int cola = msgget(key, 0666 | IPC_CREAT);
		msgctl(cola ,IPC_RMID, NULL);
	}
       
    return 0;
}
