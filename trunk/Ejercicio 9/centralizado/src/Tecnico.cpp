/* 
 * File:   Tecnico.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:39 PM
 */

#include "common/DespachadorTecnicos.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <cstdlib>
#include <sstream>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    
    // Obtengo comunicacion con el sistema de testeo
    DespachadorTecnicos despachador( config );
    std::stringstream ss;
    
    while(1) {
        try {
        Logger::debug("El tecnico entra a esperar ordenes de reparacion...", __FILE__);
            int idDispositivo = despachador.recibirOrden();
            //procesarOrden
            ss << "El tecnico recibio una orden para reparar el dispositivo " << idDispositivo;
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");
        } catch(std::string exception) {
            Logger::error("Error en el tecnico...", __FILE__);
            break;
        }
    }
    
    Logger::destroy();
    
    return 0;
}

