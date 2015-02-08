/*
 * Crea IPC's correspondientes y levanta el servidor de identificadores (RPC)
 */

#include <fstream>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "common/common.h"
#include "commonIdentificador.h"
#include "logger/Logger.h"

int main(int argc, char* argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::in);
    ipcFile.close();
    
    key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
	int semId = semget(key, 1, IPC_CREAT | 0666);
    if (semId == -1) {
        Logger::error("Error al crear el semaforo del servidor RPC", __FILE__);
        exit(1);
    }
    // Inicializo el semaforo
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *_buf;
    } arg;
    arg.val = 1;
    if (semctl(semId, 0, SETVAL, arg) == -1) {
        Logger::error("Error inicializando el semaforo", __FILE__);
    }
    
    pid_t idServer = fork();
    if (idServer == 0) {
        execlp("./idServer", "idServer", (char*)0);
        Logger::error("Error luego de ejecutar el idServer", __FILE__);
        exit(1);
    }
    
    return 0;
}