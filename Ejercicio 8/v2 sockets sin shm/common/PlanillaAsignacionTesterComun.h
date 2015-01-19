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
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

#include "logger/Logger.h"

class PlanillaAsignacionTesterComun {
public:
    PlanillaAsignacionTesterComun();
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

#endif	/* PLANILLAASIGNACIONTESTERCOMUN_H */

