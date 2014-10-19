#ifndef PLANILLA_H
#define	PLANILLA_H
#include "../ipc/Semaphore.h"
#include "planilla_local.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>


class Planilla{
private:
    Semaphore mutex_planilla_general;
    int* shm_planilla_general;
    
    Semaphore mutex_planilla_local;
    planilla_local_t* shm_planilla_local;
        
    Semaphore sem_tester_A;
    Semaphore sem_tester_B;
    
    int cola;
    
public:

    Planilla(int idTester): mutex_planilla_general(SEM_PLANILLA_GENERAL), mutex_planilla_local(SEM_PLANILLA_LOCAL + idTester), sem_tester_A(SEM_TESTER_A), sem_tester_B(SEM_TESTER_B) {
		this->mutex_planilla_general.getSem();
        this->mutex_planilla_local.getSem();
        this->sem_tester_A.getSem();
        this->sem_tester_B.getSem();
        
        key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + idTester);
        int shmlocalid = shmget(key, sizeof(planilla_local_t), 0660);
        //verificacion de errores
        this->shm_planilla_local = (planilla_local_t*)shmat(shmlocalid, NULL, 0);
        
        key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
        int shmgeneralid = shmget(key, sizeof(int), 0660);
        //verificacion de errores
        this->shm_planilla_general = (int*)shmat(shmgeneralid, NULL, 0);
        
        key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA);
        this->cola = msgget(key, IPC_CREAT);
	};
	
    int queue(){
		return this->cola;
	};
	
    void agregar(int idDispositivo){
		respuesta_lugar_t respuesta;
		respuesta.mtype = idDispositivo;
		this->mutex_planilla_general.p();
		if (*this->shm_planilla_general == MAX_DISPOSITIVOS_EN_SISTEMA){
			mutex_planilla_general.v();
			respuesta.respuesta = false;           
			msgsnd(cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0);
			return;
		}            
		(*this->shm_planilla_general)++;
		this->mutex_planilla_general.v();
		
		this->mutex_planilla_local.p();
			
		this->shm_planilla_local->cantidad++;
       
		if (this->shm_planilla_local->estadoB == LIBRE){
			this->shm_planilla_local->estadoA = OCUPADO;
			this->mutex_planilla_local.v();
		}else{
			this->shm_planilla_local->estadoA = ESPERANDO;
			this->mutex_planilla_local.v();
			this->sem_tester_A.p();
		}    
        
		respuesta.respuesta = true;
		msgsnd(this->cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0);  
	};
	
    void terminadoRequerimientoPendiente(){
		this->mutex_planilla_local.p();
        this->shm_planilla_local->estadoA = LIBRE;
        if (this->shm_planilla_local->estadoB == ESPERANDO){
            this->shm_planilla_local->estadoB = OCUPADO;
            this->sem_tester_B.v();
        }
        this->mutex_planilla_local.v();
	};
	
    void procesarSiguiente(){
		this->mutex_planilla_local.p();
        if (this->shm_planilla_local->resultados > 0){
            this->mutex_planilla_local.v();
            return;
        }
        this->shm_planilla_local->estadoB = LIBRE;
        if (this->shm_planilla_local->estadoA == ESPERANDO){
            this->shm_planilla_local->estadoA = OCUPADO;
            this->sem_tester_A.v();
        }
        this->mutex_planilla_local.v();
	};
	
    void iniciarProcesamientoResultados(){
		this->mutex_planilla_local.p();
        if (this->shm_planilla_local->resultados == 0){
            this->shm_planilla_local->estadoB = LIBRE;
            this->mutex_planilla_local.v();
            this->sem_tester_B.p();
        }else{
            this->shm_planilla_local->estadoB = OCUPADO;
            this->mutex_planilla_local.v();
        }
        this->mutex_planilla_local.p();
        this->shm_planilla_local->resultados--;
        this->mutex_planilla_local.v();
	};
	
    void eliminar(int idDispositivo){
		this->mutex_planilla_local.p();
        this->shm_planilla_local->cantidad--;
        this->mutex_planilla_local.v();
        
        this->mutex_planilla_general.p();
        (*this->shm_planilla_general)--;
        this->mutex_planilla_general.v();
	};
};

#endif	/* PLANILLA_H */

