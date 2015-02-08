/* 
 * File:   PlanillaAsignacionTesterEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 7:56 PM
 */

#pragma once

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>
#include "logger/Logger.h"
#include "common.h"

class Configuracion;

class PlanillaAsignacionTesterEspecial {
public:
    PlanillaAsignacionTesterEspecial( const Configuracion& config );
    ~PlanillaAsignacionTesterEspecial();
    
    void asignarCantTareasEspeciales(int idDispositivo, int cantTareas);
    void avisarFinEnvioTareas(int idDispositivo);
    
    bool destruirComunicacion();
private:
    Semaphore semShmemCantTesters, semShmemCantTareas;
    key_t shmemCantTestersKey, shmemCantTareasKey;
    int shmemCantTestersId, shmemCantTareasId;
    TContadorTesterEspecial* cantTestersEspecialesAsignados;
    TContadorTareaEspecial* cantTareasEspecialesAsignadas;
//Operaciones prohibidas
    PlanillaAsignacionTesterEspecial( const PlanillaAsignacionTesterEspecial& orig );
    PlanillaAsignacionTesterEspecial& operator=( const PlanillaAsignacionTesterEspecial& rv );
};
