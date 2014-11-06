/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>
#include "common/AtendedorTesters.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTesters.h"
#include "common/iPlanillaTesterA.h"
#include "logger/Logger.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {    
	srand(time(NULL));
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor y mi iPlanilla" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterA planilla(id);
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo" , nombre.str().c_str());
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
               
        if(!planilla.agregar(idDispositivo)) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
            Logger::notice("No hay lugar para atender al dispositivo!" , nombre.str().c_str());
            atendedor.enviarPrograma(idDispositivo, id,SIN_LUGAR);
            continue;
        }
        
        mensaje = "HAY lugar para atender a dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
		
		usleep(rand() % 1000 + 1000);
        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        
        Logger::notice("Se envio el programa voy a marcarle a mi iplanilla que termine de procesar el requerimiento" , nombre.str().c_str());       
        planilla.terminoRequerimientoPendiente(idDispositivo);
    }

    return 0;
}

