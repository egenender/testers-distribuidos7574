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
#include <list>
#include <sys/msg.h>
#include "errno.h"

#include "common.h"
#include "../logger/Logger.h"

using namespace std;

class AsignadorTestersEspeciales {
private:
    int msgQueueId;
    key_t key;

    typedef struct message {
        long mtype;
        int idDispositivo;
        int value;
    } TMessageAsignador;
    
public:
    
    AsignadorTestersEspeciales();
    AsignadorTestersEspeciales(const AsignadorTestersEspeciales& orig);
    virtual ~AsignadorTestersEspeciales();
    
    list<resultado_test_t> asignar(int idTester, int idDispositivo, list<int> testersEspecialesIds);

};

#endif	/* ASIGNADORTESTERSESPECIALES_H */

