/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#pragma once

#include "../logger/Logger.h"
#include "common.h"
#include "errno.h"
#include <sys/msg.h>

class Configuracion;

class AtendedorDispositivos {
private:
    int cola_requerimiento;
    int cola_tests;
    int ultimoTester;

    typedef struct message {
        long mtype;
        int idDispositivo;
        int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
    } TMessageAtendedor;

//Prohibo copia y asignacion
    AtendedorDispositivos(const AtendedorDispositivos& orig);
    AtendedorDispositivos& operator=(const AtendedorDispositivos& rv);

public:

    AtendedorDispositivos( const Configuracion& config );
    ~AtendedorDispositivos();

    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    void enviar1erRespuesta(int idDispositivo, int resultado);
    int recibirPrograma(int idDispositivo);
    void enviarResultado(int idDispositivo, int resultado); // Disp -> Tester: Grave o no grave
    int recibirOrden(int idDispositivo);
};
