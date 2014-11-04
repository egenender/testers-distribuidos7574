/* 
 * File:   Tecnico.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:39 PM
 */

/*#include <cstdlib>
#include <sstream>

*/

#include "common/DespachadorTecnicos.h"
#include "common/common.h"
#include "logger/Logger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    //Obtengo comunicacion con el sistema de testeo
    DespachadorTecnicos despachador;
    stringstream ss;

    while(true) {
        try {
            Logger::debug("El tecnico entra a esperar ordenes de reparacion...", __FILE__);
            int idDispositivo = despachador.recibirOrden();
            ss << "El tecnico recibio una orden para reparar el dispositivo " << idDispositivo;
            Logger::notice(ss.str().c_str(), __FILE__);
        } catch(std::string exception) {
            Logger::error("Error en el tecnico: " + exception, __FILE__);
            break;
        }
    }
    Logger::destroy();

    return 0;
}

