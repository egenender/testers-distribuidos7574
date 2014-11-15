#ifndef PLANILLA_H
#define	PLANILLA_H
#include "../ipc/Semaphore.h"
#include "planilla_local.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <cstdlib>
#include "../logger/Logger.h"
#include <string>

using namespace std;

class Planilla{
private:
	int id;
	std::string nombre;
	
    Semaphore mutex_planilla_general;
    int* shm_planilla_general;
    
    Semaphore mutex_planilla_local;
    planilla_local_t* shm_planilla_local;
        
    Semaphore sem_tester_A;
    Semaphore sem_tester_B;
    
    int cola;
    
public:

    Planilla(int idTester, int tipo): mutex_planilla_general(SEM_PLANILLA_GENERAL), mutex_planilla_local(SEM_PLANILLA_LOCAL + idTester), sem_tester_A(SEM_TESTER_A + idTester), sem_tester_B(SEM_TESTER_B + idTester) {
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
        
        key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA + tipo);        
        this->cola = msgget(key, 0660);
        
        id = idTester;
        stringstream ss;
        ss << "Planilla component " << id;
        nombre = ss.str();
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
			Logger::notice("No hay lugar en la planilla", nombre);
			if (msgsnd(cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0) == -1) 
				exit(0);
			return;
		}
		*shm_planilla_general = *shm_planilla_general + 1;
		stringstream ss;
		ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
		Logger::notice(ss.str(), __FILE__);
		this->mutex_planilla_general.v();
		
		this->mutex_planilla_local.p();
		this->shm_planilla_local->cantidad++;
		if (this->shm_planilla_local->estadoB == LIBRE){
			Logger::notice("El tester B esta libre, asi que el tester A puede trabajar", nombre);
			this->shm_planilla_local->estadoA = OCUPADO;
			this->mutex_planilla_local.v();
		}else{
			Logger::notice("El tester B esta ocupado, asi que A va a tener que esperar", nombre);
			this->shm_planilla_local->estadoA = ESPERANDO;
			if (this->shm_planilla_local->estadoB == ESPERANDO){ //no deberia pasar... pero quien sabe?
				this->shm_planilla_local->estadoB = OCUPADO;
				this->sem_tester_B.v();
			}
			this->mutex_planilla_local.v();
			this->sem_tester_A.p();
		}    
        
		respuesta.respuesta = true;
		if (msgsnd(this->cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0) == -1)
			exit(0);
	};
	
    void terminadoRequerimientoPendiente(){
		this->mutex_planilla_local.p();
		Logger::notice("El tester A ahora esta libre", nombre);
        this->shm_planilla_local->estadoA = LIBRE;
        if (this->shm_planilla_local->estadoB == ESPERANDO){
			Logger::notice("El tester B Estaba esperando, asi que lo libero", nombre);
            this->shm_planilla_local->estadoB = OCUPADO;
            this->sem_tester_B.v();
        }else{Logger::notice("El tester B no estaba esperando, asi que no hago nada", nombre);}
        this->mutex_planilla_local.v();
	};
	
    void procesarSiguiente(){
		this->mutex_planilla_local.p();
        if (this->shm_planilla_local->resultados > 0){
			stringstream ss;
			ss << "En procesar siguiente Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
			Logger::notice(ss.str(), nombre);
            this->mutex_planilla_local.v();
            return;
        }
        Logger::notice("En procesar siguiente, no hay resultados pendientes me fijo el estado de A", nombre);
        this->shm_planilla_local->estadoB = LIBRE;
        if (this->shm_planilla_local->estadoA == ESPERANDO){
			Logger::notice("A estaba esperando, asi que ahora va a poder trabajar", nombre);
            this->shm_planilla_local->estadoA = OCUPADO;
            this->sem_tester_A.v();
        }else{ Logger::notice("A no estaba esperando, asi que no hay nada que hacer", nombre);}
        this->mutex_planilla_local.v();
	};
	
    void iniciarProcesamientoResultados(){
		this->mutex_planilla_local.p();
        if (this->shm_planilla_local->resultados == 0){
			Logger::notice("No hay resultados pendientes, asi que el tester B queda libre", nombre);
            this->shm_planilla_local->estadoB = LIBRE;
            this->mutex_planilla_local.v();
            this->sem_tester_B.p();
        }else{
			stringstream ss;
			ss << "Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
			Logger::notice(ss.str(), nombre);
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
        *shm_planilla_general = *shm_planilla_general - 1;
        stringstream ss;
		ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
		Logger::notice(ss.str(), nombre);
        this->mutex_planilla_general.v();
	};
};

#endif	/* PLANILLA_H */

