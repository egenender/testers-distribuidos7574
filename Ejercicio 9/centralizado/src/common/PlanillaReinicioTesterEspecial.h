/* 
 * File:   PlanillaReinicioTesterEspecial.h
 * Author: ferno
 *
 * Created on November 17, 2014, 12:19 AM
 */

#pragma once

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"
#include "common.h"
#include "logger/Logger.h"

class Configuracion;

class PlanillaReinicioTesterEspecial {
public:
    PlanillaReinicioTesterEspecial( const Configuracion& config );
    ~PlanillaReinicioTesterEspecial();
    
    bool hayQueReiniciar(int idTester);
private:
    key_t msgqReinicioKey;
    int msgqReinicioId;

};
