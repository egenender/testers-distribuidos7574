/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>

#include "common/AtendedorDispositivos.h"
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
    iPlanillaTesterA planilla(id);
        
    while(true) {
        
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        
        if(!planilla.agregar()) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
            atendedor.enviarPrograma(idDispositivo, SIN_LUGAR);
            continue;
        }
        
        atendedor.enviarPrograma(idDispositivo, Programa::getPrograma());
        planilla.terminoRequerimientoPendiente();
    }

    return 0;
}

