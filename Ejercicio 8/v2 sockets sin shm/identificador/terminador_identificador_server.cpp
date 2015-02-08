/*
 * Destruyo IPCs correspondientes y remuevo el archivo
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "common/common.h"
#include "commonIdentificador.h"
#include "logger/Logger.h"

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
	int semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    remove(ipcFileName.c_str());
}