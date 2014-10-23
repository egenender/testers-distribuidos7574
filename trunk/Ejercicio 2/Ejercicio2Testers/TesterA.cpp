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
    string nombre = "TesterA ";
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    stringstream ss;
	ss << id;
    nombre += ss.str();
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterA planilla(id);
        
    while(true) {
        
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::error(mensaje + ss.str(),nombre);
        
        string output = nombre + string(": ") + mensaje + ss.str() + string("\n");
        write(1, output.c_str(), output.size());
        
        if(!planilla.agregar(idDispositivo)) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
            mensaje = "Tester A: No hay lugar para atender a dispositivo id ";
            Logger::error(mensaje + ss.str(), nombre);
            output = nombre + string(": ") + mensaje + ss.str() + string("\n");
			write(1, output.c_str(), output.size());
            atendedor.enviarPrograma(idDispositivo, id,SIN_LUGAR);
            continue;
        }
        
        mensaje = "HAY lugar para atender a dispositivo id ";
        Logger::error(mensaje + ss.str(), nombre);
        output = nombre + string(": ") + mensaje + ss.str() + string("\n");
        write(1, output.c_str(), output.size());
        
        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        
        mensaje = "Se envia el programa";
        
        Logger::error(mensaje, nombre);
        output = nombre + string(": ") + mensaje + ss.str() + string("\n");
        write(1, output.c_str(), output.size());
        
        planilla.terminoRequerimientoPendiente(idDispositivo);
    }

    return 0;
}

