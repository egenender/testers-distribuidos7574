/* 
 * File:   PlanillaAsignacionEquipoEspecial.h
 * Author: knoppix
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
private:
    const int m_MaxDispositivosEnSistema;
    Semaphore                   m_SemShmemCantTesters;
    Semaphore                   m_SemShmemCantTareas;
    int                         m_ShmemCantTestersId;
    int                         m_ShmemCantTareasId;
    TContadorTesterEspecial*    m_pCantTestersEspecialesAsignados;
    TContadorTareaEspecial*     m_pCantTareasEspecialesAsignadas;

};
