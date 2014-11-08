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

void realizarProceso(int idTester, int idDispositivo, int posicion, Planilla* planilla, AtendedorTesters* atendedor, DespachadorTesters* despachador){
	std::stringstream nombre;
	nombre << __FILE__ << " " << idTester;
	
	stringstream ss;
	ss << idDispositivo;
	string mensaje = "Envio programa de testeo especial al dispositivo id ";
    Logger::notice(mensaje + ss.str() , nombre.str().c_str());
    atendedor->enviarPrograma(idDispositivo, idTester, Programa::getPrograma());
        
    mensaje = "Espero resultado desde el dispositivo id ";
    Logger::notice(mensaje + ss.str() , nombre.str().c_str());
    resultado_test_t resul = atendedor->recibirResultado(idTester);
    mensaje = "Recibi resultado desde el dispositivo ";
    Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        
	
	int seguimiento = planilla->terminarProcesamiento(idTester, posicion, resul.result == RESULTADO_GRAVE);
        if (seguimiento == NO_CONTESTAR){
			mensaje = "Me quede esperando porque habian resultados pendientes de testeos especiales";
			Logger::notice(mensaje, nombre.str().c_str());
		}else if (seguimiento == RESULTADO_GRAVE){
			mensaje = "El resultado final es grave. Mando orden de apagado al dispositivo ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			atendedor->enviarOrden(idDispositivo, ORDEN_APAGADO, 0);
			Logger::notice(string("Le envio al tecnico la notificacion ") + ss.str(), nombre.str().c_str());
			despachador->enviarOrden(idDispositivo);
		}else if (seguimiento == RESULTADO_NO_GRAVE){
			mensaje = "El resultado final no es grave. Mando orden de reinicio al dispositivo ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			atendedor->enviarOrden(idDispositivo, ORDEN_REINICIO, 0);
		}else if(seguimiento == SEGUIR_TESTEANDO){ //SEGUIR_TESTEANDO
			mensaje = "Envio orden de seguir testeando al dispositivo ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			atendedor->enviarOrden(idDispositivo, ORDEN_SEGUIR_TESTEANDO, 0);
			mensaje = "Envio nuevo programa ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			realizarProceso(idTester, idDispositivo, posicion, planilla, atendedor, despachador);
		}else { // REPETIR_TEST
			mensaje = "Envio nuevo programa, porque hay que seguir! ";
			Logger::notice(mensaje + ss.str(), nombre.str().c_str());
			realizarProceso(idTester, idDispositivo, posicion, planilla, atendedor, despachador);
		}
}

int main(int argc, char** argv) {    
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor y despachador" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    
    srand(getpid());
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo especial" , nombre.str().c_str());
        // Espero un requerimiento
        int posicion = atendedor.recibirRequerimientoEspecial(id);
        int idDispositivo = planilla.dispositivoEnLugar(posicion);
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
                       
        realizarProceso(id, idDispositivo, posicion, &planilla, &atendedor, &despachador);
        
    }

    return 0;
}

