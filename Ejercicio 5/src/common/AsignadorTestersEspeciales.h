/* 
 * File:   DespachadorTesters.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#ifndef ASIGNADORTESTERSESPECIALES_H
#define	ASIGNADORTESTERSESPECIALES_H

#include <string>
#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "common.h"
#include "../logger/Logger.h"

class AsignadorTestersEspeciales {
private:
    int msgQueueId;
    key_t key;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageAsignador;
    
public:
    
    DespachadorTesters();
    DespachadorTesters(const DespachadorTesters& orig);
    virtual ~DespachadorTesters();
    
    void asignar(int idDispositivo, list<int> testersEspecialesIds);

};

#endif	/* ASIGNADORTESTERSESPECIALES_H */

