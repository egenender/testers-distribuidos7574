/* 
 * File:   PlanillaReinicioEquipoEspecial.h
 * Author: ferno
 *
 * Created on November 17, 2014, 1:35 PM
 */

#pragma once

#include <set>
#include <cstdlib>
#include <cstring>
#include <sys/msg.h>
#include "errno.h"
#include "logger/Logger.h"

class Configuracion;

class PlanillaReinicioEquipoEspecial {
public:
    PlanillaReinicioEquipoEspecial( const Configuracion& config );
    ~PlanillaReinicioEquipoEspecial();
    
    void avisarReinicio(const std::set<int>& idTesters, bool reiniciar);
    
private:
    key_t msgqReinicioKey;
    int msgqReinicioId;

};
