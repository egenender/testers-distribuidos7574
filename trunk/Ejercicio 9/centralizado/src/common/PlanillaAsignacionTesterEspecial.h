/* 
 * File:   PlanillaAsignacionTesterEspecial.h
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

class PlanillaAsignacionTesterEspecial {
public:
    PlanillaAsignacionTesterEspecial( const Configuracion& config );
    ~PlanillaAsignacionTesterEspecial();
    
    void asignarCantTareasEspeciales(int idDispositivo, int cantTareas);
    void avisarFinEnvioTareas(int idDispositivo);

private:
    Semaphore                   m_SemShmemCantTesters;
    Semaphore                   m_SemShmemCantTareas;
    int                         m_ShmemCantTestersId;
    int                         m_ShmemCantTareasId;
    TContadorTesterEspecial*    m_pCantTestersEspecialesAsignados;
    TContadorTareaEspecial*     m_pCantTareasEspecialesAsignadas;
//Operaciones prohibidas
    PlanillaAsignacionTesterEspecial( const PlanillaAsignacionTesterEspecial& orig );
    PlanillaAsignacionTesterEspecial& operator=( const PlanillaAsignacionTesterEspecial& rv );
};
