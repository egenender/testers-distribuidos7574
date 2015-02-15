/* 
 * File:   PlanillaAsignacionEquipoEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 7:56 PM
 */

#ifndef PLANILLAASIGNACIONEQUIPOESPECIAL_H
#define	PLANILLAASIGNACIONEQUIPOESPECIAL_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "logger/Logger.h"
#include "common/common.h"

class PlanillaAsignacionEquipoEspecial {
public:
    PlanillaAsignacionEquipoEspecial();
    virtual ~PlanillaAsignacionEquipoEspecial();
    
    void registrarTareaEspecialFinalizada(int idDispositivo);
    bool terminoTesteoEspecial(int posDispositivo, int idDispositivo);
    void reiniciarContadoresTesteoEspecial(int idDispositivo);
    void limpiarContadoresFinTesteo(int idDispositivo);
        
private:
    int shmemMsgqueueEmisor, shmemMsgqueueReceptor, shmemMsgqueueReq;
    pid_t pidEmisor, pidReceptor;
    TSharedMemoryPlanillaAsignacion memoria;
    
    void obtenerMemoriaCompartida();
    void devolverMemoriaCompartida();

};

#endif	/* PLANILLAASIGNACIONEQUIPOESPECIAL_H */

