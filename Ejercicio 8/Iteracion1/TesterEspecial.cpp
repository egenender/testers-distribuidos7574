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
#include "common/PlanillaAsignacionTesterEspecial.h"
#include "common/PlanillaReinicioTesterEspecial.h"
#include "logger/Logger.h"
#include "common/TareaEspecial.h"
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
    AtendedorTesters atendedor;
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla;
    PlanillaAsignacionTesterEspecial planillaAsignacion;
    PlanillaReinicioTesterEspecial planillaReinicio;
    
    srand(time(NULL));
    
    while(true) {
        Logger::notice("Espero por un nuevo requerimiento de testeo especial" , nombre.str().c_str());
        // Espero un requerimiento
        TMessageAssignTE msg = atendedor.recibirRequerimientoEspecial(id);
        stringstream ss;
		ss << msg.idDispositivo;
		string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        ss.clear();
        ss.str("");
        usleep( rand() % 1000 + 1000);
        
        TareaEspecial tarea;
        tarea.prepararTareasEspeciales();
        
        do {
            tarea.resetTareas();
            planillaAsignacion.asignarCantTareasEspeciales(msg.posicionDispositivo, tarea.getCantTareasEspeciales());
            for (int i = 0; i < tarea.getCantTareasEspeciales(); i++) {
                int tareaActual = tarea.getProximaTareaEspecial();
                ss << "Envio programa de testeo especial " << tareaActual << " al dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str() , nombre.str().c_str());
                ss.str("");
                if (tareaActual == -1) break;
                atendedor.enviarTareaEspecial(msg.idDispositivo, id, tareaActual, msg.posicionDispositivo);
            }

            ss << "Aviso que termine de enviar las " << tarea.getCantTareasEspeciales() << " tareas especiales";
            Logger::notice(ss.str() , nombre.str().c_str());
            ss.str("");
            planillaAsignacion.avisarFinEnvioTareas(msg.posicionDispositivo);

            usleep( rand() % 1000 + 1000);
        } while (planillaReinicio.hayQueReiniciar(id));
    }

    return 0;
}
