/* 
 * File:   PlanillaAsignacionTesterEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 7:56 PM
 */

#ifndef PLANILLAASIGNACIONTESTERESPECIAL_H
#define	PLANILLAASIGNACIONTESTERESPECIAL_H

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "logger/Logger.h"
#include "common/common.h"

class PlanillaAsignacionTesterEspecial {
public:
    PlanillaAsignacionTesterEspecial(int idTester);
    virtual ~PlanillaAsignacionTesterEspecial();
    
    void asignarCantTareasEspeciales(int idDispositivo, int cantTareas);
    void avisarFinEnvioTareas(int idDispositivo);
private:
    int idTester;
    int shmemMsgqueueEmisor, shmemMsgqueueReceptor, shmemMsgqueueReq;
    pid_t pidEmisor, pidReceptor;
    TSharedMemoryPlanillaAsignacion memoria;
    
    void obtenerMemoriaCompartida();
    void devolverMemoriaCompartida();
};

#endif	/* PLANILLAASIGNACIONTESTERESPECIAL_H */

