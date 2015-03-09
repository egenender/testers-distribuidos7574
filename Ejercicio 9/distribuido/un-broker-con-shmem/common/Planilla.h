/* 
 * File:   Planilla.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:08 PM
 * 
 * Registra cantidad de dispositivos siendo atendidos actualmente en el sistema
 */

#pragma once

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include "common/common.h"

class Configuracion;

class Planilla {
private:
    int m_ShmemMsgqueueEmisor, m_ShmemMsgqueueReceptor, m_ShmemMsgqueueReq;
    int m_IdTester;
    pid_t m_PidEmisor, m_PidReceptor;
    TSharedMemoryPlanillaGeneral m_Memoria;

    void obtenerSharedMemory();
    void devolverSharedMemory();

//Prohibido
    Planilla(const Planilla& orig);

public:
    
    Planilla( int idTester, const Configuracion& config );
    ~Planilla();

    int hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador y devuelve la posicion en las memorias privadas del sistema
    void eliminarDispositivo(int posicionDispositivo); // Simplemente decrementa en 1 el contador en la shmem

};
