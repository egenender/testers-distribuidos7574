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

#include "../logger/Logger.h"
#include "common.h"

class AtendedorDispositivos {
private:
    int cola_envios;
    int cola_recibos;
    
    int ultimoTester;
    int idDispositivo;
    
    char* connect_to;
public:

    AtendedorDispositivos(char* connect_to);
    AtendedorDispositivos(const AtendedorDispositivos& orig);    
    virtual ~AtendedorDispositivos();
        
    void enviarRequerimiento(); // Disp -> Tester
    int recibirPrograma();
    void enviarResultado(int resultado); // Disp -> Tester: Grave o no grave
    int recibirOrden(int* cant);
    int obtenerIdDispositivo();
    
    void terminar_atencion();
};

#endif	/* ATENDEDORDISPOSITIVOS_H */

