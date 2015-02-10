/* 
 * File:   PlanillaAsignacionEquipoEspecial.h
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

class PlanillaAsignacionEquipoEspecial {
public:
    PlanillaAsignacionEquipoEspecial( const Configuracion& config );
    ~PlanillaAsignacionEquipoEspecial();
    
    void registrarTareaEspecialFinalizada(int idDispositivo);
    bool terminoTesteoEspecial(int idDispositivo);
    void reiniciarContadoresTesteoEspecial(int idDispositivo);
    void limpiarContadoresFinTesteo(int idDispositivo);
    
    void initPlanilla();
    
    bool destruirComunicacion();
private:
    const int m_MaxDispositivosEnSistema;
    Semaphore semShmemCantTesters, semShmemCantTareas;
    key_t shmemCantTestersKey, shmemCantTareasKey;
    int shmemCantTestersId, shmemCantTareasId;
    TContadorTesterEspecial* cantTestersEspecialesAsignados;
    TContadorTareaEspecial* cantTareasEspecialesAsignadas;

};
