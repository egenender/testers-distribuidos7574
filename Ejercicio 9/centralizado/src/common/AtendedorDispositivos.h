/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#pragma once

#include <sys/msg.h>
#include "errno.h"
#include "../common/common.h"
#include <cstdlib>
#include <cstring>
#include "../logger/Logger.h"
#include "common.h"

class Configuracion;

class AtendedorDispositivos {
private:
    int cola_requerimiento;
    int cola_tests;
    int cola_tests_especiales;
    
    int idTester;
    int posicionDispositivo;
//Operaciones prohibidas
    AtendedorDispositivos(const AtendedorDispositivos& orig);
    AtendedorDispositivos& operator=(const AtendedorDispositivos& rv);
public:
    AtendedorDispositivos( const Configuracion& config );
    ~AtendedorDispositivos();
        
    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    int recibirPrograma(int idDispositivo);
    void enviarResultado(int idDispositivo, int resultado); // Disp -> Tester: Grave o no grave
    int recibirProgramaEspecial(int idDispositivo);
    void enviarResultadoEspecial(int idDispositivo, int resultado);
    int recibirOrden(int idDispositivo);
};
