/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>
#include "common/AtendedorTesters.h"
#include "common/DespachadorTesters.h"
#include "common/iPlanillaTesterB.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/AsignadorTestersEspeciales.h"
#include "logger/Logger.h"

using namespace std;

int main(int argc, char** argv) {
	//El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor y mi iPlanilla" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterB planilla(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo comunicacion con los testers especiales
    AsignadorTestersEspeciales asignador;
    

    while(true) {
		Logger::notice("Espero la llegada de un nuevo resultado" , nombre.str().c_str());
		planilla.iniciarProcesamientoDeResultados();
        resultado_test_t resultado = atendedor.recibirResultado(id);
        std::stringstream ss;
        ss << resultado.dispositivo;
        
        Logger::notice(std::string("Llega un nuevo resultado del dispositivo id ") + ss.str() , nombre.str().c_str());
        
        if (resultado.result == RESULTADO_INCOMPLETO) {
        	list<int> ids = Resultado::getTestersEspecialesIds(resultado.result);
        	int dispositivo = resultado.dispositivo;

        	// Se bloquea aquí esperando todos los resultados
        	list<resultado_test_t> resultados = asignador.asignar(id, resultado.dispositivo, ids);
        	resultado = Resultado::getResultadoFinal(id, dispositivo, resultados);
        }

        int orden;
        if(Resultado::esGrave(resultado.result)) {
            despachador.enviarOrden(resultado.dispositivo);
            orden = ORDEN_APAGADO;
            Logger::notice("El resultado es grave, debe apagarse el dispositivo" , nombre.str().c_str());
        } else {
            orden = ORDEN_REINICIO;
            Logger::notice("El resultado no es grave, solo debe reiniciarse el dispositivo" , nombre.str().c_str());
        }
        atendedor.enviarOrden(resultado.dispositivo,orden);
        Logger::notice("Envie orden correspondiente al dispositivo" , nombre.str().c_str());
        
        planilla.eliminarDispositivo(resultado.dispositivo);
        Logger::notice("Elimine el dispositivo de mi iplanilla" , nombre.str().c_str());
        planilla.procesarSiguiente();
    }

    return 0;
}
