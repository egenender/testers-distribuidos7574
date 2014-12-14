#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "common/common.h"
#include "ipc/Semaphore.h"

void crear_colas(int* cdt, int* ctd){
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MGSQUEUE_BROKER_DISPOSITIVOS_A_TESTERS);
	*cdt = msgget(key, 0660 | IPC_CREAT);
	
	key = ftok(ipcFileName.c_str(), MGSQUEUE_BROKER_TESTERS_A_DISPOSITIVOS);
	*ctd = msgget(key, 0660 | IPC_CREAT);
}

void atender_testers(){
	
}

void atender_dispositivos(){
	int cola_disp_a_testers;
	int cola_testers_a_disp;
	int id_mem_comunes; //Refactor: todo esto deberia ir junto, porque se va a compartir entre brokers
	int id_mem_especiales;
	int id_dispositivos_en_sistema;
	crear_colas(&cola_disp_a_testers, &cola_testers_a_disp, &id_mem_comunes, &id_mem_especiales, &id_dispositivo_en_sistema);
	
	int id = 1;
	
	int* testers_comunes = (int*) shmat(id_mem_comunes, NULL, IPC_CREAT | 0666); //Estos se manejan como una cola
	int start_comunes = 0; //esto tiene que ir en la shm?
	bool* testers_especiales = (bool*) shmat(id_mem_comunes, NULL, IPC_CREAT | 0666);
	
	
	Semaphore sem_testers(SEM_TESTERS);
	sem_testers.getSem();
	Sempahore hay_comunes(SEM_HAY_COMUNES);
	hay_comunes.getSem();
	
	//DEBERIA LANZAR SERVERS
	while (true){
		TMessageAtendedor msg;
		int ok_read = msgrcv(cola_disp_a_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), id, 0);
		if (ok_read == -1){
			exit(0);
		}
		sem_testers.p();
		if (*cant_dispositivos >= MAX_DISPOSITIVOS_EN_SISTEMA){
			//mandar mensaje de "no hay lugar"
			continue;
		}
		sem_testers.v();
		hayComunes.p();
		
		sem_testers.p();
		int tester_elegido = testers_comunes[start_comunes];
		start_comunes = (start_comunes + 1) % MAX_CANT_TESTERS_COMUNES;		
		sem_testers.v();
		
		msg.tester = tester_elegido;
		msg.mtype = tester_elegido;
		//Mandar mensaje con ese id de tester de mtype, y todo el resto de las cosas... con el mismo id de dispositivo :) (Por lo misma cola)
	}
}

void atender_testers(){
	int cola_disp_a_testers;
	int cola_testers_a_disp;
	int id_mem_comunes; //Refactor: todo esto deberia ir junto, porque se va a compartir entre brokers
	int id_mem_especiales;
	int id_dispositivos_en_sistema;
	crear_colas(&cola_disp_a_testers, &cola_testers_a_disp, &id_mem_comunes, &id_mem_especiales, &id_dispositivo_en_sistema);
	
	int id = 1;
	
	int* testers_comunes = (int*) shmat(id_mem_comunes, NULL, IPC_CREAT | 0666); //Estos se manejan como una cola
	int start_comunes = 0; //esto tiene que ir en la shm?
	bool* testers_especiales = (bool*) shmat(id_mem_comunes, NULL, IPC_CREAT | 0666);
	
	
	Semaphore sem_testers(SEM_TESTERS);
	sem_testers.getSem();
	Sempahore hay_comunes(SEM_HAY_COMUNES);
	hay_comunes.getSem();
	
	
	while(true){
		TMessageAtendedor msg;
		int ok_read = msgrcv(cola_testers_a_disp, &msg, sizeof(TMessageAtendedor) - sizeof(long), id, 0);
		if (ok_read == -1){
			exit(0);
		}
				
		//Recibe un mensaje con el aviso a los testers especiales
		for ( tester_especial in msg.testers_especiales ) {
			if (fork() == 0){
				espero por tester especial
				saco tester especial como no habilitado
				envio mensaje para que sepa que tiene que mandar programa
				exit(0);
			}
		}
		for ( tester_especial in msg.testers_especiales ){
			wait(NULL);
		}
	}
}


int main (int argc, char** argv){
	//Inicializar semaforos?
	
	if (fork() == 0){
		atender_dispositivos();
	}else{
		atender_testers();
	}
	
	return 0;
}
