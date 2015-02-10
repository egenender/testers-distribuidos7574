/* 
 * File:   PlanillaAsignacionEquipoEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 7:56 PM
 */

#ifndef PLANILLAASIGNACIONEQUIPOESPECIAL_H
#define	PLANILLAASIGNACIONEQUIPOESPECIAL_H

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

#include "logger/Logger.h"

class PlanillaAsignacionEquipoEspecial {
public:
    PlanillaAsignacionEquipoEspecial();
    virtual ~PlanillaAsignacionEquipoEspecial();
    
    void registrarTareaEspecialFinalizada(int idDispositivo);
    bool terminoTesteoEspecial(int posDispositivo, int idDispositivo);
    void reiniciarContadoresTesteoEspecial(int idDispositivo);
    void limpiarContadoresFinTesteo(int idDispositivo);
    
    void initPlanilla();
    
    bool destruirComunicacion();
private:
    Semaphore semShmemCantTesters, semShmemCantTareas;
    key_t shmemCantTestersKey, shmemCantTareasKey;
    int shmemCantTestersId, shmemCantTareasId;
    TContadorTesterEspecial* cantTestersEspecialesAsignados;
    TContadorTareaEspecial* cantTareasEspecialesAsignadas;

};

#endif	/* PLANILLAASIGNACIONEQUIPOESPECIAL_H */

