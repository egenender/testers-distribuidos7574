/* 
 * File:   iniciadorDisp.cpp
 * Author: ferno
 *
 * Created on February 7, 2015, 9:48 PM
 */

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sstream>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "common/common.h"
#include "logger/Logger.h"

using namespace std;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    if (argc != 2) {
        Logger::error("Bad arguments!", __FILE__);
        printf("Example: ./iniciadorDisp <cantDisp>");
        exit(0);
    }
    
    int cantACrear = atoi(argv[1]);
    
    // Creo IPCs
    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::out);
    ipcFile.close();
    for (int q = MSGQUEUE_ENVIO_DISP; q <= MSGQUEUE_RECEPCIONES_DISP; q++){
        key_t key = ftok(ipcFileName.c_str(), q);
        if (msgget(key, IPC_CREAT | IPC_EXCL | 0660) == -1) {
            std::stringstream ss;
            ss << "No se pudo crear una cola: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
    }
    
    //Creo dispositivos
    for (int i = 0; i < cantACrear; i++) {
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./dispositivo", "dispositivo", (char*)0);
            Logger::error("Error al ejecutar el programa dispositivo", __FILE__);
            exit(1);
        }
        usleep(1000);
    }

    Logger::notice("Dispositivos creados!", __FILE__);

    return 0;
}

