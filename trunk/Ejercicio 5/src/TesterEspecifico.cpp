/* 
 * File:   TesterEspecifico.cpp
 * Author: knoppix
 *
 * Created on October 6, 2014, 23:00 PM
 */

#include <cstdlib>

#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    // int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorDispositivos atendedor;
    
    // TODO hay que crear la interfaz de comunicacion entre tester ppal
    // y tester especifico

    while(1) {

    	int idDispositivo = testerPrincipalInterfase.recibirPedido();
        
    	bool requiereSeguirTesteo = true;
    	int idTest = 0;
    	int diagnosticoFinal = -1;

		while (requiereSeguirTesteo) {

			// Imagino que enviamos más programas,
			// se llamaban pruebas en el enunciado
			atendedor.enviarPrograma(idDispositivo, Programa::getPrograma(idTest));

			int resultado = atendedor.recibirResultado(idDispositivo);

			if (Resultado::diagnosticoEncontrado(resultado)) {
				diagnosticoFinal = Resultado::getDiagnosticoFinal(resultado);
				requiereSeguirTesteo = false;
			}
			else {
				idTest = Resultado::getSiguienteTestId(resultado);
			}
		}

		//iComunicadorConTesterPpal.enviarDiagnostico(diagnosticoFinal);
    }

    return 0;
}

