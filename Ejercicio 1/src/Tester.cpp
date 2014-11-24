/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>
#include <sstream>

#include "common/AtendedorTesters.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTesters.h"
#include "logger/Logger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    
    while(1) {
	try {        
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
	std::stringstream ss;
	ss << "El tester " << id << " recibe requerimiento del dispositivo " << idDispositivo;
	Logger::debug(ss.str().c_str(), __FILE__);
	ss.str("");
        
        if(!planilla.hayLugar()) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
	    ss << "El tester " << id << " le avisa al dispositivo " << idDispositivo << " que no hay lugar";
	    Logger::debug(ss.str().c_str(), __FILE__);
	    ss.str("");
            atendedor.enviarPrograma(idDispositivo, -1, id);
            continue;
        }
        
	ss << "El tester " << id << " le envia el programa al dispositivo " << idDispositivo;
	Logger::debug(ss.str().c_str(), __FILE__);
	ss.str("");

        atendedor.enviarPrograma(idDispositivo, Programa::getPrograma(), id);
        int resultado = atendedor.recibirResultado(id);

	ss << "El tester " << id << " recibe el resultado " << resultado << " por parte del dispositivo " << idDispositivo;
	Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
        
        if(Resultado::esGrave(resultado)) {
	    ss << "El tester " << id << " dicta que el dispositivo " << idDispositivo << " tiene una falla grave. Enviando orden";
	    Logger::debug(ss.str().c_str(), __FILE__);
	    ss.str("");
            despachador.enviarOrden(idDispositivo);
            atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO);
        } else {
	    ss << "El tester " << id << " dicta que el dispositivo " << idDispositivo << " NO tiene una falla grave. Enviando orden";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");
            atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO);
        }

	ss << "El tester " << id << " termina de testear el dispositivo " << idDispositivo << ". Eliminandolo de la planilla";
	Logger::debug(ss.str().c_str(), __FILE__);
	ss.str("");
        
        planilla.eliminarDispositivo();

	ss << "El tester " << id << " ha terminado de testear al dispositivo " << idDispositivo;
	Logger::notice(ss.str().c_str(), __FILE__);
	ss.str("");

	} catch(std::string exception) {
		Logger::error("Error en el Tester" , __FILE__);
		break;
	}
    }
    
    Logger::destroy();

    return 0;
}

