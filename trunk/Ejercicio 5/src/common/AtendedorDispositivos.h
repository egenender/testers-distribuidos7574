/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#ifndef ATENDEDORDISPOSITIVOS_H
#define	ATENDEDORDISPOSITIVOS_H

#include <sys/msg.h>
#include "errno.h"

#include "logger/Logger.h"
#include "common/common.h"

class AtendedorDispositivos {
private:
    int msgQueueId;
    key_t key;
    
    typedef struct message {
        long mtype;
        int idDispositivo;
        int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
    } TMessageAtendedor;
    
public:

    AtendedorDispositivos();
    AtendedorDispositivos(const AtendedorDispositivos& orig);    
    virtual ~AtendedorDispositivos();
    
    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    int recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int idPrograma);  // Tester -> Disp
    int recibirPrograma(int idDispositivo);
    void enviarResultado(int idDispositivo, int resultado); // Disp -> Tester: Grave o no grave
    int recibirResultado(int idDispositivo);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado
    int recibirOrden(int idDispositivo);
};

#endif	/* ATENDEDORDISPOSITIVOS_H */

