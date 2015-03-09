#pragma once

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"
#include <vector>

class Configuracion;

class AtendedorTesters {
private:

    int m_CantTestersEspeciales;
    int m_ColaEnvios;
    int m_ColaRecepcionesGeneral;
    int m_ColaRecepcionesRequerimientos;

    int m_IdTester;

    // For killing communication processes
    pid_t m_PidEmisor, m_PidReceptor;
    
    void registrarTester();

//Prohibidos
    AtendedorTesters(const AtendedorTesters& orig);

public:
    AtendedorTesters( int idTester, const Configuracion& config );
    ~AtendedorTesters();

    /**
     * Interaccion con el dispositivo
     */
    int recibirRequerimiento();
    void enviarPrograma( int idDispositivo, int tester, int idPrograma );  // Tester -> Disp
    TMessageAtendedor recibirResultado( int idTester );
    void enviarOrden( int idDispositivo, int orden ); // Tester -> Disp: Reinicio o apagado

    /**
     * Interaccion con testers especiales
     */
    void enviarAEspeciales( const std::vector<bool>& cuales, int idDispositivo, int posicionDispositivo );

    /**
     * Interaccion entre tester especial y dispositivo
     */
    void enviarTareaEspecial( int idDispositivo, int idTester, int tarea, int posicionDispositivo );

    bool destruirComunicacion();
};
