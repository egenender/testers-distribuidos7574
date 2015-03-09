/* 
 * File:   PlanillaAsignacionTesterComun.h
 * Author: knoppix
 *
 * Created on November 16, 2014, 7:56 PM
 */

#pragma once

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "logger/Logger.h"
#include "common/common.h"

class Configuracion;

class PlanillaAsignacionTesterComun {
public:
    PlanillaAsignacionTesterComun( int idTester, const Configuracion& config );
    ~PlanillaAsignacionTesterComun();

    void asignarCantTestersEspeciales(int posicionDispositivo, int cantTestersEspeciales);

private:

    int m_IdTester;
    int m_ShmemMsgqueueEmisor, m_ShmemMsgqueueReceptor, m_ShmemMsgqueueReq;
    pid_t m_PidEmisor, m_PidReceptor;
    TSharedMemoryPlanillaAsignacion m_Memoria;

    void obtenerMemoriaCompartida();
    void devolverMemoriaCompartida();
};
