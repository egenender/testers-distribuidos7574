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
#include <sys/shm.h>

#include "common/common.h"
#include "logger/Logger.h"
#include "ipc/Semaphore.h"

int main(int argc, char* argv[]) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::in);
    ipcFile.close();
    
    Semaphore semId(SEM_IDENTIFICADOR);
    semId.creaSem();
    semId.iniSem(1);
    Semaphore semTablaTestEsp(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    semTablaTestEsp.creaSem();
    semTablaTestEsp.iniSem(1);
    Semaphore semTablaTestCom(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    semTablaTestCom.creaSem();
    semTablaTestCom.iniSem(1);
    
    key_t key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTablaTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), IPC_CREAT | IPC_EXCL | 0660);
    TTablaIdTestersDisponibles* tablaTesterComDisp = (TTablaIdTestersDisponibles*) shmat(shmTablaTestCom, NULL, 0);
    for (int i = 0; i < MAX_TESTER_COMUNES; i++)    tablaTesterComDisp->disponibles[i] = true;
    
    key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    int shmTablaTestEsp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles), IPC_CREAT | IPC_EXCL | 0660);
    TTablaIdTestersEspecialesDisponibles* tablaTesterEspDisp = (TTablaIdTestersEspecialesDisponibles*) shmat(shmTablaTestEsp, NULL, 0);
    for (int i = 0; i < MAX_TESTER_ESPECIALES; i++)    tablaTesterEspDisp->disponibles[i] = true;
    
    pid_t idServer = fork();
    if (idServer == 0) {
        execlp("./idServer", "idServer", (char*)0);
        Logger::error("Error luego de ejecutar el idServer", __FILE__);
        exit(1);
    }
    
    return 0;
}
