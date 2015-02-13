/* 
 * File:   Planilla.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:08 PM
 * 
 * Registra cantidad de dispositivos siendo atendidos actualmente en el sistema
 */

#ifndef PLANILLA_H
#define	PLANILLA_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "common/common.h"

class Planilla {
private:
    int shmemMsgqueueEmisor, shmemMsgqueueReceptor;
    int idTester;
    pid_t pidEmisor, pidReceptor;
    TSharedMemoryPlanillaGeneral memoria;

    void obtenerSharedMemory();
    void devolverSharedMemory();
    
public:
    
    Planilla(int idTester);
    Planilla(const Planilla& orig);
    virtual ~Planilla();
    
    int hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador y devuelve la posicion en las memorias privadas del sistema
    void eliminarDispositivo(int posicionDispositivo); // Simplemente decrementa en 1 el contador en la shmem

};

#endif	/* PLANILLA_H */

