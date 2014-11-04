/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include "common/AtendedorTesters.h"
#include "common/DespachadorTesters.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/Planilla.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <sstream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);

    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;

    while(true) {
        try {
            // Espero un requerimiento
            int idDispositivo = atendedor.recibirRequerimiento(id);
            stringstream ss;
            ss << "El tester esp " << id << " recibió un requerimiento del dispositivo " << idDispositivo;
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            if(!planilla.hayLugar()) {
                // Si no hay lugar se le avisa con un -1 en vez de programa
                ss << "El tester esp " << id << " le avisará al dispositivo " << idDispositivo << " que no hay lugar";
                Logger::debug(ss.str().c_str(), __FILE__);
                ss.str("");
                atendedor.enviarPrograma(idDispositivo, -1);
                continue;
            }

            ss << "El tester esp " << id << " le enviará el programa al dispositivo " << idDispositivo;
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            //Envio el programa y espero el resultado
            atendedor.enviarPrograma(idDispositivo, Programa::getPrograma());
            int resultado = atendedor.recibirResultado(idDispositivo);

            ss << "El tester esp " << id << " recibió el resultado " << resultado << " del dispositivo " << idDispositivo;
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            //Proceso el resultado
            if(Resultado::esGrave(resultado)) {
                ss << "El tester esp " << id << " determinó que el dispositivo " << idDispositivo << " tiene una falla grave. Enviando orden de apagado";
                Logger::debug(ss.str().c_str(), __FILE__);
                ss.str("");
                despachador.enviarOrden(idDispositivo);
                atendedor.enviarOrden(idDispositivo, Constantes::ORDEN_APAGADO);
            } else {
                ss << "El tester esp " << id << " determinó que el dispositivo " << idDispositivo << " NO tiene una falla grave. Enviando orden de reinicio";
                Logger::debug(ss.str().c_str(), __FILE__);
                ss.str("");
                atendedor.enviarOrden(idDispositivo, Constantes::ORDEN_REINICIO);
            }

            //Elimino el dispositivo de la planilla
            ss << "El tester esp " << id << " terminó de testear el dispositivo " << idDispositivo << ". Eliminándolo de la planilla...";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            planilla.eliminarDispositivo();

            ss << "El tester esp " << id << " ha terminado de testear al dispositivo " << idDispositivo;
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");

        } catch(std::string exception) {
            Logger::error("Error en el Tester: " + exception, __FILE__);
            break;
        }
    }

    Logger::destroy();
    return 0;
}

