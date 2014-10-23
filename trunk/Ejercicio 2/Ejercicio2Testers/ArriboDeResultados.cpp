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
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    int idTester = atoi(argv[1]);
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
    
    while (true){
        resultado_test_t resultado;
        int ok_read = msgrcv(cola_lectura, &resultado, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
        if (ok_read == -1){
			exit(0);
		}
		std::cout << "RECIBI RESULTADO DEL DIPOSITIVO " << resultado.dispositivo << std::endl;
		
		int ok = msgsnd(cola_escritura, &resultado, sizeof(resultado_test_t) - sizeof(long), 0);
        if (ok == -1){
			exit(0);
		}	
		std::cout << "Escribi RESULTADO Al TesterB " << resultado.dispositivo << std::endl;
		
        mutex_planilla_local.p();
        shm_planilla_local->resultados++;
        if (shm_planilla_local->estadoB == LIBRE ){
            if (shm_planilla_local->estadoA == OCUPADO){
                shm_planilla_local->estadoB = ESPERANDO;
            }else{
                shm_planilla_local->estadoB = OCUPADO;
                sem_tester_B.v();
                std::cout << "El tester B va a poder actuar" << std::endl;
            }
        }
        mutex_planilla_local.v();
    }
    return 0;
}

