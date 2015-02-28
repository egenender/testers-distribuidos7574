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
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include "common/PlanillaVariablesDisp.h"
#include "common/Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

void destruirCola( const std::string archivoIpcs, int idCola ){
    key_t key = ftok(archivoIpcs.c_str(), idCola);
    int cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
}

void destruirColas( const std::string archivoIpcs, const Configuracion& config ){
    //Cola de dispositivos
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS) );
     //Cola de testers
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_TESTERS) );    
    //Cola de testers-dispEspeciales
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES) );
    //Cola dispositivos-config
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_CONFIG) );
    //Cola testers config    
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_TESTERS_CONFIG) );
    //Cola testers especiales    
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_TESTERS_ESPECIALES) );    
    //Cola despachador
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    //Cola reinicio testeo
    destruirCola( archivoIpcs, config.ObtenerParametroEntero(MSGQUEUE_REINICIO_TESTEO) );
}

void destruirSemaforo( const std::string& archivoIpcs, int id ){
    Semaphore sem( archivoIpcs, id );
    sem.getSem();
    sem.eliSem();
}

void destruirSemaforos( const std::string& archivoIpcs, const Configuracion& config ){
    //Semaforo
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL)); 
    //Usado por PlanillaAsignacionTesterComun, PlanillaAsignacionTesterEspecial
    //y PlanillaAsignacionEquipoEspecial
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS) );
    //Sem cola testers especiales
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_COLA_ESPECIALES));
    //Sem tareas asignadas
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS) );
    for( int i=0; i<config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA ); i++ ){
        int idDisp = i+1;
        destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_MUTEX_PLANILLA_VARS_START) + idDisp );
        destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_TE_START) + idDisp );
        destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_CV_START) + idDisp );
    }
}

void destruirSharedMemory( int idShm, const std::string& archivoIpcs, int tamanio ){
    key_t key = ftok( archivoIpcs.c_str(), idShm );
    int shmId = shmget( key, tamanio, 0660 );
    shmctl( shmId, IPC_RMID, NULL );
}

void destruirSharedMemories( const std::string& archivoIpcs, const Configuracion& config ){
    //SHM_PLANILLA_GENERAL
    destruirSharedMemory( config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL), archivoIpcs, sizeof(int) );
    //SHM_PLANILLA_GENERAL_POSICIONES
    const int maxDispositivos = config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA);
    destruirSharedMemory( config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL_POSICIONES),
                          archivoIpcs,
                          sizeof(bool) * maxDispositivos );
    //SHM_PLANILLA_CANT_TESTER_ASIGNADOS
    destruirSharedMemory( config.ObtenerParametroEntero( SHM_PLANILLA_CANT_TESTER_ASIGNADOS),
                          archivoIpcs,
                          sizeof(TContadorTesterEspecial) * maxDispositivos );
    //SHM_PLANILLA_CANT_TAREAS_ASIGNADAS
    destruirSharedMemory( config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TAREAS_ASIGNADAS),
                          archivoIpcs,
                          sizeof(TContadorTareaEspecial) * maxDispositivos );
    //SHM_PLANILLA_VARS_START
    for( int i=0; i<maxDispositivos; i++ ){
        int idDisp = i+1; //Los ids de dispositivo comienzan en 1
        destruirSharedMemory( config.ObtenerParametroEntero(SHM_PLANILLA_VARS_START) + idDisp,
                              archivoIpcs,
                              sizeof(TEstadoDispositivo) );
    }
}

int main(int argc, char** argv) {
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    destruirColas( archivoIpcs, config );    
    destruirSemaforos( archivoIpcs, config );
    destruirSharedMemories( archivoIpcs, config );

    unlink(archivoIpcs.c_str());
    return 0;
}
