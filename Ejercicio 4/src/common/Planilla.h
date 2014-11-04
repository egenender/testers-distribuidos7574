/* 
 * File:   Planilla.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:08 PM
 * 
 * Registra cantidad de dispositivos siendo atendidos actualmente en el sistema
 */

#pragma once

#include "ipc/Semaphore.h"
#include <sys/ipc.h>

class Planilla {
private:
    Semaphore m_SemShMem;
    key_t m_ShMemKey;
    int m_ShMemId;
    int* m_pCantDispositivosSiendoTesteados;

    int cantProcesosUsandoPlanilla();

//Prohibo copia y asignacion
    Planilla(const Planilla& orig);
    Planilla& operator=(const Planilla& rv);
public:
    Planilla();
    ~Planilla();

    bool hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador
    void eliminarDispositivo(); // Simplemente decrementa en 1 el contador en la shmem

    bool destruirMemoria();
    bool destruirSemaforo();

};

