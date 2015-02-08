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


int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
   
    //Semaforo y Planilla General
    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS,  0660);
    shmctl(shmgeneralid, IPC_RMID, NULL);
    
    Planilla planillaGeneral;
    if (!planillaGeneral.destruirMemoria()) {
        Logger::warn("No se pudo destruir la memoria compartida de la planilla general", __FILE__);
    }
    
    PlanillaAsignacionEquipoEspecial planillaAsignacion;
    if (!planillaAsignacion.destruirComunicacion()) {
        Logger::warn("No se pudo destruir alguna memoria compartida de la planilla de asignacion", __FILE__);
    }
    
    Semaphore semPlanillaGeneral(SEM_PLANILLA_GENERAL);
    semPlanillaGeneral.getSem();
    semPlanillaGeneral.eliSem();
		
    Semaphore sem_cola_especiales(SEM_COLA_ESPECIALES);
    sem_cola_especiales.getSem();
    sem_cola_especiales.eliSem();
    
    Semaphore semPlanillaCantTestersAsignados(SEM_PLANILLA_CANT_TESTER_ASIGNADOS);
    semPlanillaCantTestersAsignados.getSem();
    semPlanillaCantTestersAsignados.eliSem();
    
    Semaphore semPlanillaCantTareasAsignadas(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS);
    semPlanillaCantTareasAsignadas.getSem();
    semPlanillaCantTareasAsignadas.eliSem();
	
    //Destruccion de colas
    /*for (int q = MSGQUEUE_DISPOSITIVOS; q <= MSGQUEUE_ULTIMO; q++){
        key = ftok(ipcFileName.c_str(), q);
        int cola = msgget(key, 0660);
        msgctl(cola ,IPC_RMID, NULL);
    }*/
    
    unlink(ipcFileName.c_str());
    
    return 0;
}
