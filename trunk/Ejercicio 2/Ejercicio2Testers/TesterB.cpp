/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>

#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorDispositivos atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterB planilla(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTecnicos despachador;
    
    while(true) {
        planilla.iniciarProcesamientoDeResultados();
        
        int resultado = atendedor.recibirResultado(); //TODO: tambien debe darnos el id del dispositivo
        int orden;
        if(Resultado::esGrave(resultado)) {
            despachador.enviarOrden(idDispositivo);
            orden = ORDEN_APAGADO;
        } else {
            orden = ORDEN_REINICIO;
        }
        atendedor.enviarOrden(idDispositivo,orden);
        
        planilla.eliminarDispositivo(idDispositivo);
        planilla.procesarSiguiente();
    }

    return 0;
}
