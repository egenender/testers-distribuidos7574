/* 
 * File:   DespachadorTesters.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#ifndef ASIGNADORTESTERSESPECIALESB_H
#define	ASIGNADORTESTERSESPECIALESB_H

#include <string>
#include <cstring>
#include <list>
#include <sys/msg.h>
#include "errno.h"

#include "common.h"
#include "../logger/Logger.h"

using namespace std;

class AsignadorTestersEspecialesB {
private:
    int msgQueueId;
    key_t key;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
        int value;
    } TMessageAsignador;
    
public:
    
    AsignadorTestersEspecialesB();
    AsignadorTestersEspecialesB(const AsignadorTestersEspecialesB& orig);
    virtual ~AsignadorTestersEspecialesB();
    
    int recibirPedido(int idTesterEspecial);
    void enviarResultadoAlTerminar(int idTesterEspecial, int idDispositivo, int resultado);

};

#endif	/* ASIGNADORTESTERSESPECIALESB_H */

