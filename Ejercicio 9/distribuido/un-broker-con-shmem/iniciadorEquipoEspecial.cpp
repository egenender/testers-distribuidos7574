/* 
 * File:   iniciadorEquipoEspecial.cpp
 * Author: knoppix
 *
 * Created on February 12, 2015, 2:01 PM
 * 
 * Como se va a poder distribuir, el equipo especial puede ir solo en una maquina. Debe tener su 
 * iniciador propio.
 * El tecnico no se va a distribuir, por lo que se mantiene en todos los iniciadores
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "common/Planilla.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include "common/Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );

    // Creo archivo de IPCs
    std::fstream ipcFile(archivoIpcs.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
    std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();

    key_t key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }

    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_ENVIO_EQUIPO_ESPECIAL) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_ENVIO_EQUIPO_ESPECIAL) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }

    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
            
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS) );
    if (msgget(key, 0660 | IPC_CREAT) == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }

    // Creo equipo especial
    pid_t eqEspPid = fork();
    if (eqEspPid == 0) {
        execlp("./equipoEspecial", "equipoEspecial", (char*)0);
        Logger::error("Error al ejecutar el programa Equipo Especial", __FILE__);
        exit(1);
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
    sprintf(paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    sprintf(paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
    if (fork() == 0) {
        execlp( "./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
        exit(1);
    }
    
    sprintf(paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS) );
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    sprintf(paramSize, "%d", (int) sizeof(TRequerimientoSharedMemory));
    if (fork() == 0) {
        execlp( "./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPCION_REQ_SHM).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
        exit(1);
    }

    Logger::debug("Programas del equipo especial iniciados correctamente...", __FILE__);

    return 0;
}

