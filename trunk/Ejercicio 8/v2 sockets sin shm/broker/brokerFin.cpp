/* 
 * File:   brokerFin.cpp
 * Author: ferno
 *
 */

#include <cstdlib>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "common/common.h"
#include "../ipc/Semaphore.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    key_t key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    shmctl(shmTestCom, IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    int shmTestEsp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles), 0660);
    shmctl(shmTestEsp, IPC_RMID, NULL);

    Semaphore semTablaCom(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    semTablaCom.getSem();
    semTablaCom.eliSem();
    
    Semaphore semTablaEsp(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    semTablaEsp.getSem();
    semTablaEsp.eliSem();
    
    for (int i = 0; i < MAX_TESTER_ESPECIALES; i++) {
		Semaphore semEspecial(i + ID_TESTER_ESP_START);
		semEspecial.getSem();
		semEspecial.eliSem();
	}
    
    //Destruccion de colas
    for (int q = MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS; q <= MSGQUEUE_BROKER_EMISOR; q++){
        key = ftok(ipcFileName.c_str(), q);
        int cola = msgget(key, 0660);
        msgctl(cola ,IPC_RMID, NULL);
    }
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
    int queue = msgget(key, 0660);
    msgctl(queue ,IPC_RMID, NULL);
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
    queue = msgget(key, 0660);
    msgctl(queue ,IPC_RMID, NULL);
    
    unlink(ipcFileName.c_str());

    return 0;
}