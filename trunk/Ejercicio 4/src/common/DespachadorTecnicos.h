/* 
 * File:   DespachadorTecnicos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#pragma once

#include <sys/ipc.h>

class DespachadorTecnicos {
private:
    int m_MsgQueueId;
    key_t m_Key;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
    } TMessageDespachador;

//Prohibo copia y asignacion
    DespachadorTecnicos(const DespachadorTecnicos& orig);
    DespachadorTecnicos& operator=(const DespachadorTecnicos& orig);

public:

    DespachadorTecnicos();
    ~DespachadorTecnicos();

    int recibirOrden();

    bool destruirComunicacion();

};

