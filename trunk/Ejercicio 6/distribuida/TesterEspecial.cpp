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
#include "common/Planilla.h"
#include "logger/Logger.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {    
    
       
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    // El primer parametro es el id del tester
    //int id = atoi(argv[1]);
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor(TIPO_ESPECIAL);
    int id = atendedor.obtenerIdTester();
    
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor y despachador" , nombre.str().c_str());
    
    
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    
    srand(time(NULL));
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo especial" , nombre.str().c_str());
        // Espero un requerimiento
        int posicion = atendedor.recibirRequerimientoEspecial();
        int idDispositivo = planilla.dispositivoEnLugar(posicion);
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        usleep( rand() % 1000 + 1000);
        
        mensaje = "Envio programa de testeo especial al dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        atendedor.enviarPrograma(idDispositivo, Programa::getPrograma());
        
        mensaje = "Espero resultado desde el dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        int result = atendedor.recibirResultado();
        mensaje = "Recibi resultado desde el dispositivo ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        
        int seguimiento = planilla.terminarProcesamiento(posicion, result == RESULTADO_GRAVE);
        usleep( rand() % 1000 + 1000);
        if (seguimiento == NO_CONTESTAR){
			mensaje = "Aun faltan testeos por hacer, asi que yo sigo con lo mio";
			Logger::notice(mensaje, nombre.str().c_str());
		}else if (seguimiento == RESULTADO_GRAVE){
			mensaje = "El resultado final es grave. Mando orden de apagado al dispositivo ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO, 0);
			Logger::notice(string("Le envio al tecnico la notificacion ") + ss.str(), nombre.str().c_str());
			despachador.enviarOrden(idDispositivo);
		}else{
			mensaje = "El resultado final es no es grave. Mando orden de reinicio al dispositivo ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO, 0);
		}
    }
	
	atendedor.terminar_atencion(TIPO_ESPECIAL);
	
    return 0;
}

