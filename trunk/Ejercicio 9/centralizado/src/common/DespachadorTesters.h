/* 
 * File:   DespachadorTesters.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#ifndef DESPACHADORTESTERS_H
#define	DESPACHADORTESTERS_H

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
    
    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;
    
public:
    
    DespachadorTesters( const Configuracion& config );
    DespachadorTesters(const DespachadorTesters& orig);
    virtual ~DespachadorTesters();
    
    void enviarOrden(int idDispositivo);

};

#endif	/* DESPACHADORTESTERS_H */

