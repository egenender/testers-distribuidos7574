#pragma once

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>
#include "logger/Logger.h"
#include "common.h"

class Configuracion;

class PlanillaVariablesDisp {
public:
    PlanillaVariablesDisp( const Configuracion& config, int idDisp );
    ~PlanillaVariablesDisp();
    
    void iniciarCambioDeVariable( int idVar );
    void finalizarCambioDeVariable( int idVar );
    
    void iniciarTestEspecial();
    void finalizarTestEspecial();
    
    bool destruirComunicacion();
private:
    const int           m_IdDisp;
    int                 m_KeyShmemEstadoId;
    key_t               m_KeyShmemEstado;
    Semaphore           m_MutexPlanilla;
    Semaphore           m_SemTestsEspeciales;
    Semaphore           m_SemCambioVars;
    TEstadoDispositivo* m_pShmEstado;
};
