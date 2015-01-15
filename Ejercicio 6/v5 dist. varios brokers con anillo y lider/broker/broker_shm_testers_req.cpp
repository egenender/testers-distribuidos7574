#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include <string.h>


int main (void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo los ipcs necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_SHM);
	int cola_requerimiento_shm = msgget(key, 0660);
	
	Semaphore puedo_buscar(SEM_SHM_TESTERS_REQUERIMIENTO);
	puedo_buscar.getSem();
	  
	key = ftok(ipcFileName.c_str(), SHM_TESTER_QUIERE_SHM);
    int shmtester = shmget(key, sizeof(int) , 0660);
    int* tester_shm = (int*)shmat(shmtester, NULL, 0);   
	
	Semaphore sem_tester_shm(SEM_MUTEX_TESTER_QUIERE_SHM);
	sem_tester_shm.getSem();
	
    Logger::notice("Termino la obtencion de ipcs", __FILE__);
    
	/* Fin Setup */
	TMessageAtendedor msg;
	
	while (true){
		Logger::notice("Espero que haya necesidad de buscar otro requerimiento de shm", __FILE__);
		puedo_buscar.p();
		
		Logger::notice("Espero por un requerimiento de shm", __FILE__);
		int ok_read = msgrcv(cola_requerimiento_shm, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO_SHM_TESTERS, 0);
		if (ok_read == -1){
			exit(0);
		}
		
		std::stringstream ss;
		ss << "Me llega un requerimiento de shm desde tester " << msg.tester << ". Espero poder escribirlo en la shm interna con el otorgador";
		Logger::notice(ss.str(), __FILE__);
		
		sem_tester_shm.p();
		*tester_shm = msg.tester;
		sem_tester_shm.v();
		Logger::notice("Deje al otorgador la potestad de entregar shm", __FILE__);
	}
}

