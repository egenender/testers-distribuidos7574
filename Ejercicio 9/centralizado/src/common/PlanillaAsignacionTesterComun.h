/* 
 * File:   PlanillaAsignacionTesterComun.h
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

class PlanillaAsignacionTesterComun {
public:
    PlanillaAsignacionTesterComun( const Configuracion& config );
    virtual ~PlanillaAsignacionTesterComun();
    
    void asignarCantTestersEspeciales(int posicionDispositivo, int cantTestersEspeciales);
    
    bool destruirComunicacion();
    
private:
    Semaphore semShmemCantTesters, semShmemCantTareas;
    key_t shmemCantTestersKey, shmemCantTareasKey;
    int shmemCantTestersId, shmemCantTareasId;
    TContadorTesterEspecial* cantTestersEspecialesAsignados;
    TContadorTareaEspecial* cantTareasEspecialesAsignadas;

};
