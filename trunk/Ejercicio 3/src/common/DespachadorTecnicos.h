/* 
 * File:   DespachadorTecnicos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#pragma once

#include "common.h"
#include "../logger/Logger.h"
#include <cerrno>
#include <string>
#include <cstring>
#include <sys/msg.h>

class Configuracion;

class DespachadorTecnicos {
private:
    int msgQueueId;
    key_t key;

    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;

//Prohibo copia y asignacion
    DespachadorTecnicos(const DespachadorTecnicos& orig);
    DespachadorTecnicos& operator=(const DespachadorTecnicos& rv);

public:
    DespachadorTecnicos( const Configuracion& config );
    ~DespachadorTecnicos();

    int recibirOrden();
    bool destruirComunicacion();
};
