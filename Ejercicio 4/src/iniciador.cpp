/* 
 * File:   finalizador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Destruye todos los IPCs utilizados en la aplicacion
 * No mata los procesos que los usan porque destruir los ipcs
 * genera errores fatales que hacen que dichos procesos terminen.
 */

/*#include <cstdlib>
#include "unistd.h"*/

#include "logger/Logger.h"
#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/DespachadorTecnicos.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include <cerrno>
#include <cstring>

void createIPCObjects( const Configuracion& config );
void createSystemProcesses( const Configuracion& config );

int main( int argc, char** argv ){

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    Logger::notice("Logger inicializado. Inicializando IPCs...", __FILE__);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    try {
        createIPCObjects( config );
    } catch(std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        Logger::destroy();
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);

    createSystemProcesses( config );
    Logger::debug("Procesos iniciados correctamente...", __FILE__);

    Logger::notice("Sistema inicializado correctamente...", __FILE__);

    Logger::destroy();

    return 0;
}

void createIPCObjects( const Configuracion& config ) {
    const std::string archivoIpcs = config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS.c_str() );

    // Verifico que exista el archivo con las keys de los ipcs
    std::ifstream ipcFile( archivoIpcs.c_str(), std::ios::in);
    if (ipcFile.bad() || ipcFile.fail()) {
        std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();

    //Cola de mensajes entre dispositivo y testers
    AtendedorDispositivos atendedor( config );

    // Creo semaforo para la shmem de la planilla
    Semaphore semPlanilla(archivoIpcs, config.ObtenerParametroEntero( Constantes::NombresDeParametros::SEM_PLANILLA ) );
    semPlanilla.creaSem();
    semPlanilla.iniSem(1); // Inicializa el semaforo en 1
    
    // Creo la shmem de la planilla
    Planilla planilla( config );

    // Creo la cola de mensajes entre tester y tecnico
    DespachadorTecnicos despachador( config );
}

void createSystemProcesses( const Configuracion& config ) {
    // Lanzo procesos de testers
    for(int i = 0; i < config.ObtenerParametroEntero( Constantes::NombresDeParametros::CANT_TESTERS_ESP ); i++) {
        char param[3];
        int idTester = config.ObtenerParametroEntero( Constantes::NombresDeParametros::ID_TESTER_ESP_START ) + i;
        sprintf(param, "%d\n", idTester);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./tester_esp", "tester_esp", param, (char*)0);
            Logger::error("Error al ejecutar el programa tester_esp de ID " + idTester, __FILE__);
        }
    }

    //Lanzo proceso del tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./tecnico", "tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
    }

    // Lanzo procesos de dispositivos
    for(int i = 0; i < config.ObtenerParametroEntero( Constantes::NombresDeParametros::CANT_DISPOSITIVOS ); i++) {
        char param[3];
        int idDispositivo = config.ObtenerParametroEntero( Constantes::NombresDeParametros::ID_DISPOSITIVO_START ) + i;
        sprintf(param, "%d\n", idDispositivo);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./dispositivo", "dispositivo", param, (char*)0);
            Logger::error("Error al ejecutar el programa dispositivo de ID " + idDispositivo, __FILE__);
        }
    }

    Logger::debug("Programas iniciados correctamente...", __FILE__);
}

