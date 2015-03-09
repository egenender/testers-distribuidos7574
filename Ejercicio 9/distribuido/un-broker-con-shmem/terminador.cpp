/* 
 * File:   terminador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Destruye todos los IPCs utilizados en la aplicacion
 */

#include <cstdlib>
#include "common/common.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "logger/Logger.h"
#include "ipc/Semaphore.h"
#include "common/Planilla.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include "common/Configuracion.h"

using namespace Constantes::NombresDeParametros;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    key_t key;
    //Destruccion de colas
    for (int q = config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTER_COMUN); q <= config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS); q++){
        key = ftok(config.ObtenerParametroString( ARCHIVO_IPCS ).c_str(), q);
        int cola = msgget(key, 0660);
        msgctl(cola ,IPC_RMID, NULL);
    }
    
//    unlink(ipcFileName.c_str());
    
    return 0;
}
