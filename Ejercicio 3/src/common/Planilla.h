#pragma once

#include "planilla_local.h"
#include "../ipc/Semaphore.h"

class Planilla{
private:
    Semaphore mutex_planilla_general;
    int* shm_planilla_general;

    Semaphore mutex_planilla_local;
    planilla_local_t* shm_planilla_local;

    Semaphore sem_tester_primero;
    Semaphore sem_tester_segundo;
    Semaphore sem_tester_resultado;

    int cola;

public:
    Planilla(int tester);
    int queue();
    void agregar(int idDispositivo);
    void terminadoRequerimientoPendiente();
    void procesarSiguiente();
    void iniciarProcesamientoResultados();
    void iniciarProcesamientoResultadosParciales();

    void eliminar(int idDispositivo);
};

