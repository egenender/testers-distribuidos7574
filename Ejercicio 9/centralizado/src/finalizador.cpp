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

void destruirObjetosIPCsPlanillaGeneral( const std::string& archivoIpcs, const Configuracion& config ){
    //Semaforo
    Semaphore semPlanillaGeneral( archivoIpcs,
                                  config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL));
    semPlanillaGeneral.getSem();
    semPlanillaGeneral.eliSem();
    //Shm planilla
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL) );
    int shmId = shmget( key, sizeof(int), 0660 );
    shmctl( shmId, IPC_RMID, NULL );
    //Shm posiciones
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL_POSICIONES) );
    int maxDispositivosEnSistema = config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA);
    shmId = shmget( key, sizeof(bool) * maxDispositivosEnSistema, 0660 );
    shmctl( shmId, IPC_RMID, NULL );
}

void destruirSemaforo( const std::string& archivoIpcs, int id ){
    Semaphore sem( archivoIpcs, id );
    sem.getSem();
    sem.eliSem();
}

int main(int argc, char** argv) {
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    //Planilla general
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );    
    destruirObjetosIPCsPlanillaGeneral( archivoIpcs, config );
    
    //Usado por PlanillaAsignacionTesterComun, PlanillaAsignacionTesterEspecial
    //y PlanillaAsignacionEquipoEspecial
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS) );

    //Cola de dispositivos
    const int msgQueueDispositivos = config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS);
    key_t key = ftok(archivoIpcs.c_str(), msgQueueDispositivos);
    int cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola de testers
    const int msgQueueTesters = config.ObtenerParametroEntero(MSGQUEUE_TESTERS);
    key = ftok(archivoIpcs.c_str(), msgQueueTesters);
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola de testers-dispEspeciales
    const int msgQueueDispTestersEsp = config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES);
    key = ftok(archivoIpcs.c_str(), msgQueueDispTestersEsp);
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola dispositivos-config
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_CONFIG) );
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola testers config
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_TESTERS_CONFIG) );
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola testers especiales
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_TESTERS_ESPECIALES) );
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola testers especiales
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_DESPACHADOR) );
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Sem cola testers especiales
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_COLA_ESPECIALES));
    //Sem tareas asignadas
    destruirSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS) );

    //Shm planilla cant testers asignados
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero( SHM_PLANILLA_CANT_TESTER_ASIGNADOS) );
    int maxDispositivosEnSistema = config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA );
    int shmId = shmget( key, sizeof(TContadorTesterEspecial) * maxDispositivosEnSistema, 0660 );
    shmctl( shmId, IPC_RMID, NULL );

    






    PlanillaAsignacionEquipoEspecial planillaAsignacion( config );
    //if (!planillaAsignacion.destruirComunicacion()) {
    //    Logger::warn("No se pudo destruir alguna memoria compartida de la planilla de asignacion", __FILE__);
    //}

    

    //IPCs de planillas de variables
    for( int i=0; i<config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA ); i++ ){
        //Semaforos de planilla variables
        Semaphore semPlanillaVarsTE( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_TE_START) + i );
        semPlanillaVarsTE.getSem();
        semPlanillaVarsTE.eliSem();
        Semaphore semPlanillaVarsCV( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_CV_START) + i );
        semPlanillaVarsCV.getSem();
        semPlanillaVarsCV.eliSem();        
        Semaphore semMutexPlanillaVars( archivoIpcs, config.ObtenerParametroEntero(SEM_MUTEX_PLANILLA_VARS_START) + i );
        semMutexPlanillaVars.getSem();
        semMutexPlanillaVars.eliSem();
        //Shared memory
        PlanillaVariablesDisp planillaVars( config, i+1 );
        planillaVars.destruirComunicacion();
    }
    
    unlink(archivoIpcs.c_str());
    
    return 0;
}
