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
#include <sys/shm.h>
#include <errno.h>
#include <cstring>

class Configuracion;

class Planilla {
private:
    const int m_MaxDispositivosEnSistema;
    Semaphore semShMem;
    key_t shMemKey, shMemPosicionesKey;
    int shMemId, shMemPosicionesId;
    int* cantDispositivosSiendoTesteados;
    bool* idsPrivadosDispositivos;
    
    int cantProcesosUsandoPlanilla();
//Prohibo copia y asignacion
    Planilla(const Planilla& orig);
    Planilla& operator=( const Planilla& rv );
public:
    
    Planilla( const Configuracion& config );    
    ~Planilla();
    
    int hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador y devuelve la posicion en las memorias privadas del sistema
    void eliminarDispositivo(int posicionDispositivo); // Simplemente decrementa en 1 el contador en la shmem

    bool destruirMemoria();
    bool destruirSemaforo();
    
    void initPlanilla();

};
