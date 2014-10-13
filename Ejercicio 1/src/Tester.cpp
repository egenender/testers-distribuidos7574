/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>

#include "common/AtendedorTesters.h"
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
    // int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    // Obtengo comunicacion con los tecnicos
    DespachadorTecnicos despachador;
    
    while(1) {
        
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        
        if(!planilla.hayLugar()) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
            atendedor.enviarPrograma(idDispositivo, -1);
            continue;
        }
        
        atendedor.enviarPrograma(idDispositivo, Programa::getPrograma());
        int resultado = atendedor.recibirResultado(idDispositivo);
        
        if(Resultado::esGrave(resultado)) {
            despachador.enviarOrden(idDispositivo);
            atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO);
        } else {
            atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO);
        }
        
        planilla.eliminarDispositivo();
    }
    
    Logger::destroy();

    return 0;
}

