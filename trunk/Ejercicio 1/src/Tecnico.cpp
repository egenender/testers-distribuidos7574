/* 
 * File:   Tecnico.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:39 PM
 */

#include <cstdlib>

#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"
#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // Obtengo comunicacion con el sistema de testeo
    DespachadorTecnicos despachador;
    
    while(1) {
        int idDispositivo = despachador.recibirOrden();
        Logger::notice("Tecnicos recibieron orden para reparar dispositivo" + idDispositivo, __FILE__);
    }
    
    Logger::destroy();
    
    return 0;
}

