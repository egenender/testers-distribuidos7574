/* 
 * File:   AtendedorEquipoEspecial.h
 * Author: ferno
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
    ~AtendedorEquipoEspecial();
    
    TResultadoEspecial recibirResultadoEspecial();
    void enviarFinTestEspecialADispositivo(int idDispositivo);
    void enviarOrden(int idDispositivo, int orden);
private:
    int colaDispTesterEsp, colaReinicioTestEsp, colaOrdenDispositivos;
//Operaciones prohibidas
    AtendedorEquipoEspecial( const AtendedorEquipoEspecial& orig );
    AtendedorEquipoEspecial& operator=( const AtendedorEquipoEspecial& rv );
};
