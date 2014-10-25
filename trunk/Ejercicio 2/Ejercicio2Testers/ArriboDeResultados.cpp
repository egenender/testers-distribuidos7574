/* 
 * File:   ArriboDeResultados.cpp
 * Author: knoppix
 *
 * Created on October 11, 2014, 4:17 PM
 */

#include <cstdlib>
#include "ipc/Semaphore.h"
#include "common/planilla_local.h"
#include <sys/msg.h>
#include "errno.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "logger/Logger.h"


using namespace std;

int main(int argc, char** argv) {
	int idTester = atoi(argv[1]);
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << idTester;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
	
    Semaphore mutex_planilla_local(SEM_PLANILLA_LOCAL + idTester);
    mutex_planilla_local.creaSem();
    
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + idTester);
    int shmlocalid = shmget(key, sizeof(planilla_local_t), 0660);
    planilla_local_t* shm_planilla_local = (planilla_local_t*)shmat(shmlocalid, NULL, 0);
    
    Semaphore sem_tester_B(SEM_TESTER_B + idTester);
    sem_tester_B.getSem();
    
    //Las colas parecerian estar al reves, pero lo que hacen es justamente ponerlo
    //en la cola correcta
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ESCRITURA_RESULTADOS);
    int cola_lectura = msgget(key, 0660);
    key = ftok(ipcFileName.c_str(), MSGQUEUE_LECTURA_RESULTADOS);
    int cola_escritura = msgget(key, 0660);
   
   Logger::notice("Procesamiento inicial completo, ejecutando ciclo principal" , nombre.str().c_str());
   
    while (true){
        resultado_test_t resultado;
        int ok_read = msgrcv(cola_lectura, &resultado, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
        if (ok_read == -1){
			exit(0);
		}
		std::stringstream ss;
		ss << "Se recibe resultado de testeo desde el dispositivo " << resultado.dispositivo;
		Logger::notice(ss.str() , nombre.str().c_str());
				
		int ok = msgsnd(cola_escritura, &resultado, sizeof(resultado_test_t) - sizeof(long), 0);
        if (ok == -1){
			exit(0);
		}	
		Logger::notice("Puse el resultado en la cola correcta para que lo lea el tester B correspondiente", nombre.str().c_str());
		
        mutex_planilla_local.p();
        shm_planilla_local->resultados++;
        Logger::notice("Aumente la cantidad de resultados pendientes de procesar para este tester", nombre.str().c_str());
        if (shm_planilla_local->estadoB == LIBRE ){
			Logger::notice("El tester B esta libre, asi que reviso la activida del tester A", nombre.str().c_str());
            if (shm_planilla_local->estadoA == OCUPADO){
				Logger::notice("El tester A esta ocupado, asi que el Tester B estara esperando para que el A lo despierte", nombre.str().c_str());
                shm_planilla_local->estadoB = ESPERANDO;
            }else{
				Logger::notice("El tester A NO esta ocupad, Asi que el Tester B ya puede actuar", nombre.str().c_str());
                shm_planilla_local->estadoB = OCUPADO;
                sem_tester_B.v();
            }
        }
        mutex_planilla_local.v();
    }
    return 0;
}

