/* 
 * File:   ArriboDeResultados.cpp
 * Author: knoppix
 *
 * Created on October 11, 2014, 4:17 PM
 */

#include <cstdlib>
#include "ipc/Semaphore.h"
#include "common/planilla_local.h"

using namespace std;

int main(int argc, char** argv) {
    int idTester = argv[0];
    Semaphore mutex_planilla_local(SEM_PLANILLA_LOCAL + atoi(argv[0]));
    mutex_planilla_local.creaSem();
    
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + atoi);
    int shmlocalid = shmget(key, sizeof(planilla_local_t), 0660);
    planilla_local_t* shm_planilla_local = (planilla_local_t*)shmat(shmlocalid, NULL, 0);
    
    Semaphore sem_tester_B(SEM_TESTER_B);
    
    //Las colas parecerian estar al reves, pero lo que hacen es justamente ponerlo
    //en la cola correcta
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ESCRITURA_RESULTADOS);
    int cola_lectura = msgget(key, IPC_CREAT);
    key = ftok(ipcFileName.c_str(), MSGQUEUE_LECTURA_RESULTADOS);
    int cola_escritura = msgget(key, IPC_CREAT);
    
    while (true){
        resultado_test_t resultado;
        msgrcv(cola_lectura, &resultado, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
        mutex_planilla_local.p();
        shm_planilla_local->resultados++;
        msgsnd(cola_escritura, &resultado, sizeof(resultado_test_t) - sizeof(long), 0);
        if (shm_planilla_local->estadoB == LIBRE ){
            if (shm_planilla_local->estadoA == OCUPADO){
                shm_planilla_local->estadoB = ESPERANDO;
            }else{
                shm_planilla_local->estadoB = OCUPADO;
                sem_tester_B.v();
            }
        }
        mutex_planilla_local.v();
    }
    return 0;
}

