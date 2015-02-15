/* 
 * File:   iniciador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Crea todos los IPCs a usar e inicia todos los procesos correspondientes a la aplicacion
 */

#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "common/Planilla.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"

void createIPCObjects();
void createSystemProcesses();

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Inicializando IPCs...", __FILE__);
    
    try {
        createIPCObjects();
    } catch(std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        Logger::destroy();
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);
    
    createSystemProcesses();
    Logger::debug("Procesos iniciados correctamente...", __FILE__);
    
    Logger::notice("Sistema inicializado correctamente...", __FILE__);
    
    Logger::destroy();
    
    return 0;
}

void createIPCObjects() {

    // Creo el archivo que se usara para obtener las keys
    std::fstream ipcFile(ipcFileName.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
	std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();
    
    //creacion de colas
    for (int q = MSGQUEUE_ENVIO_TESTER_COMUN; q <= MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS; q++) {
        key_t key = ftok(ipcFileName.c_str(), q);
        if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1) {
            std::cout << "No se pudo crear una cola: " << strerror(errno) << std::endl;
        }
    }
}

void createSystemProcesses() {

    // Creo testers
    for(int i = 0; i < CANT_TESTERS_COMUNES; i++) {
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
			// Inicio el programa correspondiente
            execlp("./testerComun", "testerComun", (char*)0);
            Logger::error("Error al ejecutar el programa TesterComun", __FILE__);
            exit(1);
        }
    }
    
    if (fork() == 0){
		execlp("./distribuidorMsgTester", "distribuidorMsgTester", (char*) 0);
        Logger::error("No se ejecutó correctamente el distribuidor de mensajes", __FILE__);
        exit(1);
	}

    for(int i = 0; i < CANT_TESTERS_ESPECIALES; i++) {
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
			// Inicio el programa correspondiente
            execlp("./testerEspecial", "testerEspecial", (char*)0);
            Logger::error("Error al ejecutar el programa TesterEspecial", __FILE__);
            exit(1);
        }
    }

    // Creo al tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./tecnico", "tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
        exit(1);
    }
    
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];
    
    // Se crea el emisor de requerimientos de shmem
    sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    sprintf(paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION,
				paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}
    
    // Se crea el emisor de requerimientos de shmem
    sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL);
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    sprintf(paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));
	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL,
				paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}
    
    // Se crea el emisor de requerimientos de shmem
    sprintf(paramIdCola, "%d", MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS);
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    sprintf(paramSize, "%d", (int) sizeof(TRequerimientoSharedMemory));
	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPCION_REQ_PLANILLAS,
				paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}

    Logger::debug("Programas iniciados correctamente...", __FILE__);
}
