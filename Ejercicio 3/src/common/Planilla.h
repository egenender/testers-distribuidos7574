#pragma once

#include "planilla_local.h"
#include "../ipc/Semaphore.h"

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
    int m_MaxDispositivosLocales;
    
//Prohibo copia y asignacion
    Planilla(const Planilla& orig);
    Planilla& operator=(const Planilla& rv);
public:
    Planilla(int tester, const Configuracion& config);
    int queue();
    void agregar(int idDispositivo);
    void terminadoRequerimientoPendiente();
    void procesarSiguiente();
    void iniciarProcesamientoResultados();
    void iniciarProcesamientoResultadosParciales();

    void eliminar(int idDispositivo);
    bool destruirCola();
    bool destruirMemoriaGeneral();
    bool destruirMemoriaLocal();
    bool destruirSemaforoGeneral();
    bool destruirSemaforosLocales( std::string& msjError );
};

