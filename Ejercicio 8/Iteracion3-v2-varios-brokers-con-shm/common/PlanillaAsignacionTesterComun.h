/* 
 * File:   PlanillaAsignacionTesterComun.h
 * Author: ferno
 *
 * Created on November 16, 2014, 7:56 PM
 */

#ifndef PLANILLAASIGNACIONTESTERCOMUN_H
#define	PLANILLAASIGNACIONTESTERCOMUN_H

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "logger/Logger.h"
#include "common/common.h"

class PlanillaAsignacionTesterComun {
public:
    PlanillaAsignacionTesterComun(int idTester);
    virtual ~PlanillaAsignacionTesterComun();
    
    void asignarCantTestersEspeciales(int posicionDispositivo, int cantTestersEspeciales);
    
private:

    int idTester;
    int shmemMsgqueueEmisor, shmemMsgqueueReceptor, shmemMsgqueueReq;
    pid_t pidEmisor, pidReceptor;
    TSharedMemoryPlanillaAsignacion memoria;
    
    void obtenerMemoriaCompartida();
    void devolverMemoriaCompartida();
};

#endif	/* PLANILLAASIGNACIONTESTERCOMUN_H */

