#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"


int main (void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo los ipcs necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	int cola_requerimiento_especiales = msgget(key, 0660);
	
	Semaphore sem_comunes(SEM_CANT_TESTERS_COMUNES);
	sem_comunes.getSem();
	
	key = ftok(ipcFileName.c_str(), SHM_TABLA_TESTERS);
    int shmtabla = shmget(key, sizeof(tabla_testers_disponibles_t) , IPC_CREAT | 0660);
    tabla_testers_disponibles_t* tabla = (tabla_testers_disponibles_t*)shmat(shmtabla, NULL, 0);
    
    Semaphore sem_tabla(SEM_TABLA_TESTERS);
    sem_tabla.getSem();
    
    Logger::notice("Termino la obtencion de ipcs", __FILE__);
    
	/* Fin Setup */
	TMessageAtendedor msg;
	int cant_atendidos = 0;
	
	while (true){
		if (cant_atendidos == 5){
			wait(NULL);
			cant_atendidos--;
		}
		Logger::notice("Espero por un requerimiento especial", __FILE__);
		int ok_read = msgrcv(cola_requerimiento_especiales, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO_ESPECIAL, 0);
		if (ok_read == -1){
			exit(0);
		}
		
		std::stringstream ss;
		Logger::notice("Me llega un requerimiento especial", __FILE__);
		cant_atendidos++;
		if (fork() != 0) continue;
		
		for (int i = 0; i < MAX_TESTERS_ESPECIALES; i++){
			if (fork() == 0 ){
				if (! msg.especiales[i]) exit(0);
				ss << "Espero a que este disponible el tester especial id " << i + ID_TESTER_ESPECIAL_START;
				Logger::notice(ss.str(), __FILE__);
				
				Semaphore sem_especial(SEM_ESPECIAL_DISPONIBLE + i);
				sem_especial.getSem();
				sem_especial.p();
				
				ss << "El tester especial " << i + ID_TESTER_ESPECIAL_START << " ya esta disponible";
				Logger::notice(ss.str(), __FILE__);
				
				sem_tabla.p();
				tabla->testers_especiales[i] = 0;
				sem_tabla.v();
				
				msg.mtype = i + ID_TESTER_ESPECIAL_START;
				int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
				if(ret == -1) {
					exit(1);
				}
				
				exit(0);
			}
		}	
		
		for (int i = 0; i < MAX_TESTERS_ESPECIALES; i++){
			wait(NULL);
		}
		exit(0);
	}
}
