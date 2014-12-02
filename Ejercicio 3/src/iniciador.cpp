/* 
 * File:   iniciador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Crea todos los IPCs a usar e inicia todos los procesos correspondientes a la aplicacion
 */

#include "logger/Logger.h"
#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/DespachadorTecnicos.h"
#include "common/Configuracion.h"
#include "ipc/Semaphore.h"
#include <cstdlib>
#include <fstream>
#include <common/common.h>
#include "unistd.h"
#include <errno.h>

/*
 * 
 */

void createIPCObjects(const Configuracion& config);
void createSystemProcesses(const Configuracion& config);
void createExternProcesses(const Configuracion& config);

int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Inicializando IPCs...", __FILE__);

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    try {
        createIPCObjects(config);
    } catch (std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        Logger::destroy();
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);

    createSystemProcesses(config);
    createExternProcesses(config);
    Logger::debug("Procesos iniciados correctamente...", __FILE__);

    Logger::notice("Sistema inicializado correctamente...", __FILE__);

    Logger::destroy();

    return 0;
}

void createIPCObjects(const Configuracion& config) {
    const std::string archivoIpcs = config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS.c_str());

    // Creo el archivo que se usara para obtener las keys
    std::fstream ipcFile(Constantes::ARCHIVO_LOG.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
        std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();

    // Creo semaforo para la shmem general de la planilla
    Semaphore semPlanilla(archivoIpcs, config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_GENERAL));
    semPlanilla.creaSem();
    semPlanilla.iniSem(1); // Inicializa el semaforo en 1

    std::stringstream ss;
    ss << "Se crep el semaforo para la shem general con ID " << semPlanilla.getSemId();
    Logger::error(ss.str().c_str(), __FILE__);
    ss.str("");

    // Creo las shmem y semaforos de las planillas
    int iTesterStart = config.ObtenerParametroEntero(Constantes::NombresDeParametros::ID_TESTER_START);
    int cantTesters = config.ObtenerParametroEntero(Constantes::NombresDeParametros::CANT_TESTERS);
    for (int iTester = iTesterStart; iTester < iTesterStart + cantTesters; iTester++) {

        Semaphore semPlanillaLocal(archivoIpcs, config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_LOCAL) + iTester);
        semPlanillaLocal.creaSem();
        semPlanillaLocal.iniSem(1); // Inicializa el semaforo en 1
        ss << "Se creo el semaforo para la shem local con ID " << semPlanillaLocal.getSemId();
        Logger::error(ss.str().c_str(), __FILE__);
        ss.str("");

        Semaphore semTester1ro(archivoIpcs, config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_A) + iTester);
        semTester1ro.creaSem();
        semTester1ro.iniSem(1); // Inicializa el semaforo en 1
        ss << "Se creo el semaforo, para el tester 1, con ID " << semTester1ro.getSemId();
        Logger::error(ss.str().c_str(), __FILE__);
        ss.str("");


        Semaphore semTester2do(archivoIpcs, config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_B) + iTester);
        semTester2do.creaSem();
        semTester2do.iniSem(0); // Inicializa el semaforo en 1
        ss << "Se creo el semaforo, para el tester 2, con ID " << semTester2do.getSemId();
        Logger::error(ss.str().c_str(), __FILE__);
        ss.str("");

        Semaphore semTesterRta(archivoIpcs, config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_RESULTADO) + iTester);
        semTesterRta.creaSem();
        semTesterRta.iniSem(0); // Inicializa el semaforo en 1
        ss << "Se creo el semaforo,para el tester de resultados, con ID " << semTesterRta.getSemId();
        Logger::error(ss.str().c_str(), __FILE__);
        ss.str("");

        Planilla planilla(iTester, config);

    }

    // Creo la cola de mensajes entre tester y tecnico
    DespachadorTecnicos despachador(config);
}

void lanzarProceso(const std::string& nombre, char* param, int id) {
    pid_t newPid = fork();
    if (newPid < 0) {
        Logger::error("Error al hacer fork de " + nombre);
        exit(-1);
    } else if (newPid == 0) {
        execlp(std::string("./" + nombre).c_str(), nombre.c_str(), param, (char*) 0);
        std::stringstream ss;
        ss << "Error al ejecutar el " << nombre << " de ID " << id;
        Logger::error(ss.str().c_str(), __FILE__);
        ss.str("");
        exit(-2);
    }
}

void createSystemProcesses(const Configuracion& config) {

    // Creo testers
    for (int i = 0; i < config.ObtenerParametroEntero(Constantes::NombresDeParametros::CANT_TESTERS); i++) {
        char param[3];
        int idTester = config.ObtenerParametroEntero(Constantes::NombresDeParametros::ID_TESTER_START) + i;
        sprintf(param, "%d", idTester);
        //Subprocesos
        lanzarProceso("tester", param, idTester);
        lanzarProceso("tester_2do", param, idTester);
        lanzarProceso("tester_respuesta", param, idTester);
        lanzarProceso("arribo_de_resultados", param, idTester);
        lanzarProceso("arribo_de_resultados_parciales", param, idTester);
        lanzarProceso("planilla_tester_1ro", param, idTester);
        lanzarProceso("planilla_tester_2do", param, idTester);
        lanzarProceso("planilla_tester_rta", param, idTester);
    }

    // Creo al tecnico
    pid_t tecPid = fork();
    if (tecPid == 0) {
        execlp("./tecnico", "tecnico", (char*) 0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
    }
    Logger::debug("Programas del sistema iniciados correctamente...", __FILE__);

}

void createExternProcesses(const Configuracion& config) {
    
        // Creo dispositivos
    for (int i = 0; i < config.ObtenerParametroEntero(Constantes::NombresDeParametros::CANT_DISPOSITIVOS); i++) {
        char param[3];
        int idDispositivo = config.ObtenerParametroEntero(Constantes::NombresDeParametros::ID_DISPOSITIVO_START) + i;
        sprintf(param, "%d\n", idDispositivo);
        pid_t newPid = fork();
        if (newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./dispositivo", "dispositivo", param, (char*) 0);
            Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
        }
    }
     Logger::debug("Dispositivos iniciados correctamente...", __FILE__);
}