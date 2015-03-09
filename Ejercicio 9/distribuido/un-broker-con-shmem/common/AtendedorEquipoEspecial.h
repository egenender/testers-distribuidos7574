/* 
 * File:   AtendedorEquipoEspecial.h
 * Author: knoppix
 *
 * Created on November 17, 2014, 12:59 AM
 */

#pragma once

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "common/common.h"
#include "logger/Logger.h"

class Configuracion;

class AtendedorEquipoEspecial {
public:
    AtendedorEquipoEspecial( const Configuracion& config );
    virtual ~AtendedorEquipoEspecial();
    
    TMessageAtendedor recibirResultadoEspecial();
    void enviarFinTestEspecialADispositivo(int idDispositivo);
    void enviarOrden(int idDispositivo, int orden);
private:
    int m_ColaRecepciones, m_ColaEnvios;

    // For killing communication processes
    pid_t m_PidEmisor, m_PidReceptor;
};
