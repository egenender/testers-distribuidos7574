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
#include "../common/common.h"
#include <cstdlib>
#include <cstring>

#include "../logger/Logger.h"
#include "common.h"
#include "identificador/identificador.h"

class AtendedorDispositivos {
private:
    int colaEnvios;
    int colaRecepciones;

    int idTester;
    int posicionDispositivo;
    int idDispositivo;

public:

    AtendedorDispositivos(int idDispositivo);
    AtendedorDispositivos(const AtendedorDispositivos& orig);    
    virtual ~AtendedorDispositivos();
        
    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    int recibirPrograma(int idDispositivo);
    void enviarResultado(int idDispositivo, int resultado); // Disp -> Tester: Grave o no grave
    int recibirProgramaEspecial(int idDispositivo);
    void enviarResultadoEspecial(int idDispositivo, int resultado);
    int recibirOrden(int idDispositivo);
};

#endif	/* ATENDEDORDISPOSITIVOS_H */

