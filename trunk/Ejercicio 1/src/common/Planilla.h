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

#include "ipc/Semaphore.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

class Planilla {
private:
    Semaphore semShMem;
    key_t shMemKey;
    int shMemId;
    int* cantDispositivosSiendoTesteados;
    
    int cantProcesosUsandoPlanilla();
    
public:
    
    Planilla();
    Planilla(const Planilla& orig);
    virtual ~Planilla();
    
    bool hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador
    void eliminarDispositivo(); // Simplemente decrementa en 1 el contador en la shmem

};

#endif	/* PLANILLA_H */

