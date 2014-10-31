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
#include "logger/Logger.h"
#include "common/Planilla.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {    
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor, despachador y Planilla" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    
    srand(time(NULL));
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo" , nombre.str().c_str());
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
                          	
        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        Logger::notice(string("Envie programa a dispositivo ") + ss.str(), nombre.str().c_str());
        
        resultado_test_t resul = atendedor.recibirResultado(id);
        Logger::notice(string("Recibi resultado del dispositivo ") + ss.str(), nombre.str().c_str());
        
        if (resul.result == RESULTADO_GRAVE){
			Logger::notice(string("Le envio orden de apagado al dispositivo ") + ss.str(), nombre.str().c_str());
			atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO, 0);
			Logger::notice(string("Le envio al tecnico la notificacion ") + ss.str(), nombre.str().c_str());
			despachador.enviarOrden(idDispositivo);
		}else if (resul.result == SEGUIR_TESTEANDO){
			int cant_testers = rand() % (MAXIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION - MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION) + MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION;	
			Logger::notice(string("Le envio orden de seguir evaluando al dispositivo ") + ss.str(), nombre.str().c_str());
			atendedor.enviarOrden(idDispositivo, ORDEN_SEGUIR_TESTEANDO, cant_testers);
			ss.str("");
			ss << cant_testers;
			Logger::notice(string("Le envio requerimientos a los ") + ss.str() + string(" testers especiales! ") + ss.str(), nombre.str().c_str());
			atendedor.enviarAEspeciales(cant_testers, planilla.setRequerimiento(idDispositivo, cant_testers));
		}else{
			atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO, 0);
		}
               
    }

    return 0;
}

