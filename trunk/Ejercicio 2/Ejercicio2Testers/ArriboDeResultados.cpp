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
    int idTester = 
    Semaphore mutex_planilla_local(SEM_PLANILLA_LOCAL + atoi(argv[0]));
    mutex_planilla_local.creaSem();
    
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + atoi);
    int shmlocalid = shmget(key, sizeof(planilla_local_t), 0660);
    shm_planilla_local = shmat(shmlocalid, NULL, 0);
    
    planilla_local_t* shm_planilla_local;
    Semaphore sem_tester_B(SEM_TESTER_B);
    
    
    
    while (true){
        //TODO: leer de la cola
        mutex_planilla_local.p();
        shm_planilla_local->resultados++;
        //TODO: escribir a la cola
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

