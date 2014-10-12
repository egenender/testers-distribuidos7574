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
#include <common/common.h>
#include "unistd.h"

#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/DespachadorTecnicos.h"

/*
 * 
 */

void createIPCObjects();
void createSystemProcesses();

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    Logger::error("Logger inicializado. Inicializando IPCs...", __FILE__);

    try {
        createIPCObjects();
    } catch(std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);
    
    createSystemProcesses();
    Logger::debug("Procesos iniciados correctamente...", __FILE__);
    
    Logger::notice("Sistema inicializado correctamente...", __FILE__);
    
    return 0;
}

void createIPCObjects() {

    // Creo el archivo que se usara para obtener las keys
    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::in | std::ios_base::out);
    ipcFile.close();
    
    // Cola de mensajes entre dispositivo y testers
    AtendedorDispositivos atendedor;
    
    // Creo semaforo para la shmem de la planilla
    Semaphore semPlanilla(SHMEM_PLANILLA);
    semPlanilla.creaSem();
    
    // Creo la shmem de la planilla
    Planilla planilla;
    
    // Creo la cola de mensajes entre tester y tecnico
    DespachadorTecnicos despachador;
    
}

void createSystemProcesses() {

    // Creo dispositivos
    for(int i = 0; i < CANT_DISPOSITIVOS; i++) {
        char param[3];
        int idDispositivo = ID_DISPOSITIVO_START;
        sprintf(param, "%d\n", idDispositivo++);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./dispositivo", "dispositivo", param, (char*)0);
            Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
        }
    }
    
    // Creo testers
    for(int i = 0; i < CANT_TESTERS; i++) {
        char param[3];
        int idTester = ID_TESTER_START;
        sprintf(param, "%d\n", idTester++);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./tester", "tester", param, (char*)0);
            Logger::error("Error al ejecutar el programa tester de ID" + idTester, __FILE__);
        }
    }
    
    // Creo al tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./tecnico", "tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
    }
    
    Logger::debug("Programas iniciados correctamente...", __FILE__);
    
}