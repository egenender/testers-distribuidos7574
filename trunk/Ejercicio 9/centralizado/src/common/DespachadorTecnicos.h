/* 
 * File:   DespachadorTecnicos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#pragma once

#include <string>
#include <cstring>
#include <sys/msg.h>
#include "errno.h"
#include "common.h"
#include "../logger/Logger.h"

class Configuracion;

class DespachadorTecnicos {
private:
    int     m_MsgQueueId;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;
    
//Operaciones prohibidas
    DespachadorTecnicos(const DespachadorTecnicos& orig);
    DespachadorTecnicos& operator=(const DespachadorTecnicos& rv);
public:
    
    DespachadorTecnicos( const Configuracion& config );
    ~DespachadorTecnicos();
    
    int recibirOrden();
};
