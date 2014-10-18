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
#include "common/DespachadorTesters.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterB planilla(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    
    while(true) {
        planilla.iniciarProcesamientoDeResultados();
        
        resultado_test_t resultado = atendedor.recibirResultado(id);
        int orden;
        if(Resultado::esGrave(resultado)) {
            despachador.enviarOrden(resultado.dispositivo);
            orden = ORDEN_APAGADO;
        } else {
            orden = ORDEN_REINICIO;
        }
        atendedor.enviarOrden(resultado.dispositivo,orden);
        
        planilla.eliminarDispositivo(resultado.dispositivo);
        planilla.procesarSiguiente();
    }

    return 0;
}
