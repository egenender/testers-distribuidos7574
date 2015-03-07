/*
 * Destruyo IPCs correspondientes y remuevo el archivo
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "common/common.h"
#include "logger/Logger.h"

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
	int semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok(ipcFileName.c_str(), SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
	semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok(ipcFileName.c_str(), SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
	semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTablaTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    shmctl(shmTablaTestCom, IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    int shmTablaTestEsp = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    shmctl(shmTablaTestEsp, IPC_RMID, NULL);
    
//    remove(ipcFileName.c_str());
}
