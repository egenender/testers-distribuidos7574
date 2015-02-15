/* 
 * File:   AtendedorEquipoEspecial.h
 * Author: ferno
 *
 * Created on November 17, 2014, 12:59 AM
 */

#ifndef ATENDEDOREQUIPOESPECIAL_H
#define	ATENDEDOREQUIPOESPECIAL_H

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "common/common.h"
#include "logger/Logger.h"

class AtendedorEquipoEspecial {
public:
    AtendedorEquipoEspecial();
    virtual ~AtendedorEquipoEspecial();
    
    TMessageAtendedor recibirResultadoEspecial();
    void enviarFinTestEspecialADispositivo(int idDispositivo);
    void enviarOrden(int idDispositivo, int orden);
private:
    int colaRecepciones, /*colaReinicioTestEsp, */colaEnvios;
    
    int idBroker;

    // For killing communication processes
    pid_t pidEmisor, pidReceptor;
    
    void registrarEquipoEspecial();
};

#endif	/* ATENDEDOREQUIPOESPECIAL_H */

