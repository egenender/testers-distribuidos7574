/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>
#include <list>

#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"
#include "common/DespachadorTesters.h"

using namespace std;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterB planilla(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    //Obtengo comunicacion con testers especiales
    iAsignadorTestersEspeciales asignadorEsp(id);
    
    while(true) {
        planilla.iniciarProcesamientoDeResultados();
        
        resultado_test_t resultado = atendedor.recibirResultado(id);

        if (Resultado::requiereMasPruebas(resultado)) {
        	//Cuando se detecta que requiere mas pruebas, se puede asignar a varios
        	//testers especiales para que realicen N pruebas cada uno
        	list<int> tiposDeTestEspeciales = Resultado::getTiposDeTestEspeciales(resultado);
        	asignadorEsp.asignar(tiposDeTestEspeciales);

        	//Me bloqueo hasta obtene todos los diagnosticos
   	        list<int> diagnosticos = asignadorEsp.getTodosLosDiagnosticos();
   	        resultado = Resultado::getDiagnosticoFinal(diagnosticos);
        }

        int orden;
        if(Resultado::esGrave(resultado)) {
            despachador.enviarOrden(resultado.dispositivo);
            orden = ORDEN_APAGADO;
        } else {
            orden = ORDEN_REINICIO;
        }
        atendedor.enviarOrden(resultado.dispositivo,orden);
        
        planilla.eliminarDispositivo(resultado.dispositivo);
        planilla.procesarSiguiente();
    }

    return 0;
}
