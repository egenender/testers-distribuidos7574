/* 
 * File:   Tester.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:46 PM
 */

#include "common/AtendedorTesters.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTesters.h"
#include "logger/Logger.h"
#include "common/Planilla.h"
#include "common/PlanillaAsignacionTesterComun.h"
#include "common/Configuracion.h"
#include <string>
#include <cstdlib>
#include <vector>

using namespace Constantes::NombresDeParametros;
using namespace std;

int main(int argc, char** argv) {
    srand(time(NULL));
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << id;
    Logger::notice("Inicia el procesamiento, cargo el atendedor, despachador y Planilla" , nombre.str().c_str());
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor( config );
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador( config );
    // Obtengo planilla general de sync con otros tester
    Planilla planilla( config );
    PlanillaAsignacionTesterComun planillaAsignacion( config );

    srand(time(NULL));
    
    const int idPrimerTesterEspecial = config.ObtenerParametroEntero(ID_TESTER_ESPECIAL_START);

    while(true) {
        Logger::notice("Espero por un nuevo requerimiento de testeo" , nombre.str().c_str());
        // Espero un requerimiento
        TMessageAtendedor msgRequerimiento = atendedor.recibirRequerimiento();
        int idDispositivo = msgRequerimiento.idDispositivo;
        int tipoDispositivo = msgRequerimiento.tipoDispositivo;
        stringstream ss;
        ss << msgRequerimiento.idDispositivo;
        string mensaje = "Recibido requerimiento desde dispositivo id ";
        Logger::notice(mensaje + ss.str() , nombre.str().c_str());
        
        int posicionDispositivo = planilla.hayLugar();
        if (posicionDispositivo == SIN_LUGAR){
            string mensaje = "No hay lugar para atender al dispositivo id ";
            Logger::notice(mensaje + ss.str() , nombre.str().c_str());
            atendedor.enviarPrograma(idDispositivo, id, SIN_LUGAR);
            continue;
        }

        usleep( rand() % 1000 + 1000);
        Logger::notice(string("Envio programa a dispositivo ") + ss.str(), nombre.str().c_str());                  	
        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        
        Logger::notice(string("Espero resultado de dispositivo ") + ss.str(), nombre.str().c_str());                  	    

        resultado_test_t resul = atendedor.recibirResultado(id);
        Logger::notice(string("Recibi resultado del dispositivo ") + ss.str(), nombre.str().c_str());
        usleep( rand() % 1000 + 1000);

        if (resul.result == RESULTADO_GRAVE){
            Logger::notice(string("Le envio orden de apagado al dispositivo ") + ss.str(), nombre.str().c_str());
            atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO);
            Logger::notice(string("Le envio al tecnico la notificacion ") + ss.str(), nombre.str().c_str());
            despachador.enviarOrden(idDispositivo);
            planilla.eliminarDispositivo(posicionDispositivo);
        } else if (resul.result == SEGUIR_TESTEANDO){
            int cant_testers = 0;
            const int cantTestersEspeciales = config.ObtenerParametroEntero(CANT_TESTERS_ESPECIALES);
            vector<bool> los_testers( cantTestersEspeciales );
            while (cant_testers < 2 || cant_testers > 4){ //requerimientos
                cant_testers = 0;
                for (int i = 0; i < cantTestersEspeciales; i++){
                    int random = rand() % 2;
                    cant_testers += random;
                    los_testers[i] = random;
                }
            }

            Logger::notice(string("Le envio orden de seguir evaluando al dispositivo ") + ss.str(), nombre.str().c_str());
            atendedor.enviarOrden(idDispositivo, ORDEN_SEGUIR_TESTEANDO);
            ss.str("");
            ss << "Le envio los requerimientos a los " << cant_testers << " testers especiales, ";
            for (int i = 0; i < cantTestersEspeciales; i++){
                    if(los_testers[i])
                            ss << i+idPrimerTesterEspecial << " ";
            }
            Logger::notice(ss.str() , nombre.str().c_str());
            planillaAsignacion.asignarCantTestersEspeciales(posicionDispositivo, cant_testers);
            atendedor.enviarAEspeciales(los_testers, idDispositivo, posicionDispositivo);
            atendedor.enviarReqTestConfig( id, idDispositivo, tipoDispositivo );
        }else{
            atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO);
            planilla.eliminarDispositivo(posicionDispositivo);
        }
    }

    return 0;
}

