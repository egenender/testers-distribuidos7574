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
using std::string;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
   
    //Semaforo y Planilla General
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL) );
    const int cantResultados = config.ObtenerParametroEntero(CANT_RESULTADOS);
    int shmgeneralid = shmget(key, sizeof(resultado_t) * cantResultados,  0660);
    shmctl(shmgeneralid, IPC_RMID, NULL);
    
    Planilla planillaGeneral( config );
    if (!planillaGeneral.destruirMemoria()) {
        Logger::warn("No se pudo destruir la memoria compartida de la planilla general", __FILE__);
    }
    
    PlanillaAsignacionEquipoEspecial planillaAsignacion( config );
    if (!planillaAsignacion.destruirComunicacion()) {
        Logger::warn("No se pudo destruir alguna memoria compartida de la planilla de asignacion", __FILE__);
    }
    
    Semaphore semPlanillaGeneral( archivoIpcs,
                                  config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL));
    semPlanillaGeneral.getSem();
    semPlanillaGeneral.eliSem();

    Semaphore sem_cola_especiales( archivoIpcs,
                                   config.ObtenerParametroEntero(SEM_COLA_ESPECIALES));
    sem_cola_especiales.getSem();
    sem_cola_especiales.eliSem();
    
    Semaphore semPlanillaCantTestersAsignados( archivoIpcs,
                                               config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS) );
    semPlanillaCantTestersAsignados.getSem();
    semPlanillaCantTestersAsignados.eliSem();
    
    Semaphore semPlanillaCantTareasAsignadas( archivoIpcs,
                                              config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS) );
    semPlanillaCantTareasAsignadas.getSem();
    semPlanillaCantTareasAsignadas.eliSem();

    //Destruccion de colas de dispositivos
    const int msgQueueDispositivos = config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS);
    const int msgQueueUltimo = config.ObtenerParametroEntero(MSGQUEUE_ULTIMO);
    for (int q = msgQueueDispositivos; q <= msgQueueUltimo; q++){
        key = ftok(archivoIpcs.c_str(), q);
        int cola = msgget(key, 0660);
        msgctl(cola ,IPC_RMID, NULL);
    }
    //Cola dispositivos-config
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_CONFIG) );
    int cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);
    //Cola testers config
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(MSGQUEUE_TESTERS_CONFIG) );
    cola = msgget(key, 0660);
    msgctl(cola ,IPC_RMID, NULL);

    //Semaforos de planilla variables
    for( int i=0; i<config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA ); i++ ){
        Semaphore semPlanillaVarsTE( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_TE_START) + i );
        semPlanillaVarsTE.getSem();
        semPlanillaVarsTE.eliSem();
        Semaphore semPlanillaVarsCV( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_CV_START) + i );
        semPlanillaVarsCV.getSem();
        semPlanillaVarsCV.eliSem();        
        Semaphore semMutexPlanillaVars( archivoIpcs, config.ObtenerParametroEntero(SEM_MUTEX_PLANILLA_VARS_START) + i );
        semMutexPlanillaVars.getSem();
        semMutexPlanillaVars.eliSem();
    }
    
    unlink(archivoIpcs.c_str());
    
    return 0;
}
