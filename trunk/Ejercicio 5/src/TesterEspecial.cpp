/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include <cstdlib>
#include "common/AtendedorTestersEspeciales.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/AsignadorTestersEspecialesB.h"
#include "logger/Logger.h"
#include "common/common.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {    
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor y despachador" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTestersEspeciales atendedor;
    // Obtengo comunicacion con los testerB
    AsignadorTestersEspecialesB asignador;
    
    srand(time(NULL));
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo especial" , nombre.str().c_str());
        // Espero un requerimiento
        int idDispositivo = asignador.recibirPedido(id);
        stringstream ss;
		ss << idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        
        bool seguirTesteando = true;
        
        while (seguirTesteando) {
            mensaje = "Envio programa de testeo especial al dispositivo id ";
            Logger::notice(mensaje + ss.str() , nombre.str().c_str());
            atendedor.enviarPruebaEspecial(idDispositivo, id, Programa::getPrograma());

            mensaje = "Espero resultado de la prueba especial desde el dispositivo id ";
            Logger::notice(mensaje + ss.str() , nombre.str().c_str());
            resultado_test_t resultado = atendedor.recibirResultadoPruebaEspecial(id);

            mensaje = "Recibi resultado de la prueba especial desde el dispositivo ";
            Logger::notice(mensaje + ss.str() , nombre.str().c_str());

            int sigPrueba = Resultado::getSiguientePrueba(resultado.result);
            if (sigPrueba == FIN_SEGUIR_TESTEANDO) {
            	seguirTesteando = false;
            }
        }
        asignador.enviarResultadoAlTerminar(id, idDispositivo, rand() % 2);
    }

    return 0;
}

