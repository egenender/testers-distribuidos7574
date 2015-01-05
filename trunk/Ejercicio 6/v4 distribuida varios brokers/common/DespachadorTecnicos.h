/* 
 * File:   DespachadorTecnicos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#ifndef DESPACHADORTECNICOS_H
#define	DESPACHADORTECNICOS_H

#include <string>
#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "common.h"
#include "../logger/Logger.h"

class DespachadorTecnicos {
private:
    int msgQueueId;
    key_t key;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;
    
public:
    
    DespachadorTecnicos();
    DespachadorTecnicos(const DespachadorTecnicos& orig);
    virtual ~DespachadorTecnicos();
    
    int recibirOrden();

    bool destruirComunicacion();

};

#endif	/* DESPACHADORTECNICOS_H */

