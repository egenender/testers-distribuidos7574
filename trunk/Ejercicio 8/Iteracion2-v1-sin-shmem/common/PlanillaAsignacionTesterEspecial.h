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
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

#include "logger/Logger.h"

class PlanillaAsignacionTesterEspecial {
public:
    PlanillaAsignacionTesterEspecial();
    virtual ~PlanillaAsignacionTesterEspecial();
    
    void asignarCantTareasEspeciales(int idDispositivo, int cantTareas);
    void avisarFinEnvioTareas(int idDispositivo);
    
    bool destruirComunicacion();
private:
    Semaphore semShmemCantTesters, semShmemCantTareas;
    key_t shmemCantTestersKey, shmemCantTareasKey;
    int shmemCantTestersId, shmemCantTareasId;
    TContadorTesterEspecial* cantTestersEspecialesAsignados;
    TContadorTareaEspecial* cantTareasEspecialesAsignadas;

};

#endif	/* PLANILLAASIGNACIONTESTERESPECIAL_H */

