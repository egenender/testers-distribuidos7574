/* 
 * File:   DespachadorTesters.h
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

class DespachadorTesters {
private:
    int m_MsgQueueId;

    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;
    
//Prohibido
    DespachadorTesters(const DespachadorTesters& orig);

public:

    DespachadorTesters( const Configuracion& config );
    ~DespachadorTesters();

    void enviarOrden(int idDispositivo);
};
