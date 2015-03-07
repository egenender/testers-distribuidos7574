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
#include "common/PlanillaAsignacionTesterComun.h"
#include "identificador/identificador.h"
#include <string>

using namespace std;

int getIdTesterComun();
int desregistrarTesterComun(int id);

int main(int argc, char** argv) {    
	srand(time(NULL));
    // El primer parametro es el id del tester
    int id = getIdTesterComun();
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    if (id == 0) {
        Logger::error("No hay lugar en el sistema para este tester comun", __FILE__);
        exit(1);
    }
    
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargo el atendedor, despachador y Planilla" , nombre.str().c_str());
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    // Obtengo planilla general de sync con otros tester
    Planilla planilla(id);
    PlanillaAsignacionTesterComun planillaAsignacion(id);
    
    srand(time(NULL));
    
    while(true) {
		Logger::notice("Espero por un nuevo requerimiento de testeo" , nombre.str().c_str());
        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();
        stringstream ss;
		ss << idDispositivo;
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
  
        TMessageAtendedor resul = atendedor.recibirResultado(id);
        Logger::notice(string("Recibi resultado del dispositivo ") + ss.str(), nombre.str().c_str());
        usleep( rand() % 1000 + 1000);

        if (resul.value == RESULTADO_GRAVE){
            Logger::notice(string("Le envio orden de apagado al dispositivo ") + ss.str(), nombre.str().c_str());
            atendedor.enviarOrden(idDispositivo, ORDEN_APAGADO);
            Logger::notice(string("Le envio al tecnico la notificacion ") + ss.str(), nombre.str().c_str());
            despachador.enviarOrden(idDispositivo);
            planilla.eliminarDispositivo(posicionDispositivo);
        } else if (resul.value == SEGUIR_TESTEANDO) {
            int cant_testers = 0;
            bool los_testers[CANT_TESTERS_ESPECIALES];
            while (cant_testers < 2 || cant_testers > 4){ //requerimientos
                cant_testers = 0;
                for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++) {
                    int random = rand() % 2;
                    cant_testers += random;
                    los_testers[i] = random;
                }
            }
			
            Logger::notice(string("Le envio orden de seguir evaluando al dispositivo ") + ss.str(), nombre.str().c_str());
            atendedor.enviarOrden(idDispositivo, ORDEN_SEGUIR_TESTEANDO);
            ss.str("");
            ss << "Le envio los requerimientos a los " << cant_testers << " testers especiales, ";
            for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
                    if(los_testers[i])
                            ss << i + ID_TESTER_ESP_START << " ";
            }
            Logger::notice(ss.str() , nombre.str().c_str());
            planillaAsignacion.asignarCantTestersEspeciales(posicionDispositivo, cant_testers);
            atendedor.enviarAEspeciales(los_testers, idDispositivo, posicionDispositivo);
            atendedor.enviarReqTestConfig( id, idDispositivo, tipoDispositivo );
        } else {
            atendedor.enviarOrden(idDispositivo, ORDEN_REINICIO);
            planilla.eliminarDispositivo(posicionDispositivo);
        }
               
    }
    
    desregistrarTesterComun(id);

    return 0;
}

int getIdTesterComun() {

    CLIENT *clnt;
    int  *result_2;
    char *getidtestercomun_1_arg;

    clnt = clnt_create (UBICACION_SERVER_IDENTIFICADOR, IDENTIFICADORPROG, IDENTIFICADORVERS, "udp");
    if (clnt == NULL) {
        Logger::error("Error en la creación del cliente RPC", __FILE__);
        clnt_pcreateerror (UBICACION_SERVER_IDENTIFICADOR);
        exit (1);
    }
    
    result_2 = getidtestercomun_1((void*)&getidtestercomun_1_arg, clnt);
    if (result_2 == (int *) NULL) {
        Logger::error("Error en la llamada al RPC obteniendo el ID", __FILE__);
        clnt_perror (clnt, "call failed");
    }

    clnt_destroy (clnt);
    
    return *result_2;
}