/* 
 * File:   AtendedorTestersEspeciales.h
 * Author: knoppix
 *
 * Created on February 5, 2015, 1:02 AM
 */

#pragma once

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"
#include "identificador/identificador.h"

class Configuracion;

class AtendedorTestersEspeciales {
private:
    int m_ColaEnvios, m_ColaRecepcionesReq, m_ColaRecepcionesReinicio;
    int m_IdTester;

    // For killing communication processes
    pid_t m_PidEmisor, m_PidReceptor;

    void registrarTester();

//Prohibido
    AtendedorTestersEspeciales(const AtendedorTestersEspeciales& orig);

public:
    AtendedorTestersEspeciales( int idTester, const Configuracion& config );
    ~AtendedorTestersEspeciales();
    
    TMessageAtendedor recibirRequerimientoEspecial(int idEsp);
    void enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo);
    void enviarDisponibilidad();
};
