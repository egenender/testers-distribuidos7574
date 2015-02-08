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
    
    pid_t idServer = fork();
    if (idServer == 0) {
        execlp("./idServer", "idServer", (char*)0);
        Logger::error("Error luego de ejecutar el idServer", __FILE__);
        exit(1);
    }
    
    return 0;
}
