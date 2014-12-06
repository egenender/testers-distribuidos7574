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
    int msgQueueId;
    key_t key;
    

//Prohibo copia y asignacion
    DespachadorTesters(const DespachadorTesters& orig);
    DespachadorTesters& operator=(const DespachadorTesters& rv);
public:    
    DespachadorTesters( const Configuracion& config );
    ~DespachadorTesters();

    void enviarOrden(int idDispositivo);
};
