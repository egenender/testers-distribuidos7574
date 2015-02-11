#ifndef ATENDEDORTESTERS_H
#define	ATENDEDORTESTERS_H

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"

class AtendedorTesters {
private:

    int colaEnvios;
    int colaRecepcionesGeneral;
    int colaRecepcionesRequerimientos;
    
    int idTester;
    
    // For killing communication processes
    pid_t pidEmisor, pidReceptor;
    
    void registrarTester();
    
public:
    AtendedorTesters(int idTester);
    AtendedorTesters(const AtendedorTesters& orig);
    virtual ~AtendedorTesters();
    
    /**
     * Interaccion con el dispositivo
     */
    int recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int tester, int idPrograma);  // Tester -> Disp
    TMessageAtendedor recibirResultado(int idTester);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado

    /**
     * Interaccion con testers especiales
     */
    void enviarAEspeciales(bool cuales[], int idDispositivo, int posicionDispositivo);

    /**
     * Interaccion entre tester especial y dispositivo
     */
    void enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo);

    bool destruirComunicacion();
};

#endif	/* ATENDEDORTESTERS_H */

