/* 
 * File:   AtendedorTestersEspeciales.h
 * Author: ferno
 *
 * Created on February 5, 2015, 1:02 AM
 */

#ifndef ATENDEDORTESTERSESPECIALES_H
#define	ATENDEDORTESTERSESPECIALES_H

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"
#include "identificador/identificador.h"

class AtendedorTestersEspeciales {
private:
    int colaEnvios, colaRecepciones;
    int idTester, idBroker;
    
    // For killing communication processes
    pid_t pidEmisor, pidReceptor;
    
    void registrarTester();

public:
    AtendedorTestersEspeciales(int idTester);
    //AtendedorTestersEspeciales(const AtendedorTestersEspeciales& orig);
    virtual ~AtendedorTestersEspeciales();
    
    TMessageAtendedor recibirRequerimientoEspecial(int idEsp);
    void enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo);
    void enviarDisponibilidad();

};

#endif	/* ATENDEDORTESTERSESPECIALES_H */

