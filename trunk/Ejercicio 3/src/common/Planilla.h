#pragma once

#include "../ipc/Semaphore.h"
#include "planilla_local.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <cstdlib>
#include "../logger/Logger.h"
#include <string>

class Configuracion;

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
    int m_IdShmLocal;
    int m_IdShmGeneral;
    int id;
    std::stringstream ss;
    
//Prohibo copia y asignacion
    Planilla(const Planilla& orig);
    Planilla& operator=(const Planilla& rv);
public:
    Planilla(int idTester, const Configuracion& config);
    int queue();
    void agregar(int idDispositivo);
    void terminadoRequerimientoPendiente();
    void procesarSiguienteResultado();
    void procesarSiguienteResultadoParcial();
    void iniciarProcesamientoResultados();
    void iniciarProcesamientoResultadosParciales();

    void eliminar(int idDispositivo);
    
    bool destruirCola();
    bool destruirMemoriaGeneral();
    bool destruirMemoriaLocal();
    bool destruirSemaforoGeneral();
    bool destruirSemaforosLocales( std::string& msjError );

};

