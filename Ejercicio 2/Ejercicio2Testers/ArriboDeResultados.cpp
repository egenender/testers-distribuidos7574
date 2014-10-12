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
    Semaphore mutex_planilla_local;
    planilla_local_t* shm_planilla_local;
    Semaphore sem_tester_B;
    
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

