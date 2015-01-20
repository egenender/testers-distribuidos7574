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
   
	key_t key;
	//Semaforo y Planilla General
    /*key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS,  0660);
    shmctl(shmgeneralid, IPC_RMID, NULL);*/
    
   	
    //Destruccion de colas
    for (int q = MSGQUEUE_DISPOSITIVOS_ENVIOS; q <= MSGQUEUE_BROKER_SHM_TESTERS; q++){
		key = ftok(ipcFileName.c_str(), q);
		int cola = msgget(key, 0660);
		msgctl(cola ,IPC_RMID, NULL);
	}
		
	for (int i = SEM_SHM_TESTERS_REQUERIMIENTO; i < SEM_ESPECIAL_DISPONIBLE + MAX_TESTERS_ESPECIALES; i++){
		Semaphore sem_especial(i);
		sem_especial.getSem();
		sem_especial.eliSem();
	}
	
	key = ftok(ipcFileName.c_str(), SHM_TABLA_TESTERS);
    int shmtabla = shmget(key, sizeof(tabla_testers_disponibles_t) , 0660);
    shmctl(shmtabla, IPC_RMID, NULL);
        
	key = ftok(ipcFileName.c_str(), SHM_TESTER_QUIERE_SHM);
    int shmtester = shmget(key, sizeof(int) , 0660);
    shmctl(shmtester, IPC_RMID, NULL);
	
	key = ftok(ipcFileName.c_str(), SHM_NEXT_BROKER);
    int shmnext = shmget(key, sizeof(int) , 0660);
    shmctl(shmnext, IPC_RMID, NULL);
	
	key = ftok(ipcFileName.c_str(), SHM_LIDER);
    int shmlider = shmget(key, sizeof(int),  0660);
    shmctl(shmlider, IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), SHM_VERSION);
    int shmversion = shmget(key, sizeof(int),  0660);
    shmctl(shmversion, IPC_RMID, NULL);
	
    return 0;
}
