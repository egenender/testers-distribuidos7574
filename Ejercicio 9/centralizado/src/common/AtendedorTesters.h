#ifndef ATENDEDORTESTERS_H
#define	ATENDEDORTESTERS_H

#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"
#include "../ipc/Semaphore.h"

class AtendedorTesters {
private:
    int cola_requerimiento;
    int cola_recibos_tests;
    int cola_testers_especiales;
    int cola_tareas_especiales;
    Semaphore sem_cola_especiales;
    
public:
    AtendedorTesters();
    AtendedorTesters(const AtendedorTesters& orig);
    virtual ~AtendedorTesters();
    
    int recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int tester, int idPrograma);  // Tester -> Disp
    resultado_test_t recibirResultado(int idTester);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado
    void enviarAEspeciales(bool cuales[], int idDispositivo, int posicionDispositivo);
    TMessageAssignTE recibirRequerimientoEspecial(int idEsp);
    void enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo);
    
    void enviarReqTestConfig( int idDispositivo, int idTester, int tipoDispositivo );

    bool destruirComunicacion();

};

#endif	/* ATENDEDORTESTERS_H */

