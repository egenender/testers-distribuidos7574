/* 
 * File:   Dispositivo.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:12 PM
 * 
 * El dispositivo interactua con el sistema de testeo, enviando requerimientos, recibiendo programas, enviando resultados
 * y esperando ordenes
 */

#include "common/AtendedorDispositivos.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <cstdlib>
#include <sstream>

using namespace std;

void correrTestEspecial( PlanillaVariablesDisp& planillaVars ){
    planillaVars.iniciarTestEspecial();
    usleep( rand() % 1000 + 1000 );
    planillaVars.finalizarTestEspecial();
}

int main(int argc, char** argv) {
    srand(time(NULL));
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    // Por parametro se recibe el ID del dispositivo
    int id = atoi(argv[1]);

    std::stringstream ss;
    ss << "El dispositivo " << id << " se crea";
    Logger::debug(ss.str().c_str(), __FILE__);
    ss.str("");
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    
    // Comunicacion con el sistema de testeo
    AtendedorDispositivos atendedor( config );
    //Para exclusion mutua entre tests especiales y cambios de variable de config
    PlanillaVariablesDisp planillaVariables( config );
    // TODO: Log
    
    while(true) {
    try {
        // Dispositivo envia requerimiento
        ss << "El dispositivo " << id << " envia un requerimiento al sistema de testeo";
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
        atendedor.enviarRequerimiento(id);
        // Recibe programa, verificando que no sea un rechazo por parte del sistema
        int program = atendedor.recibirPrograma(id);
        if (program == SIN_LUGAR) {
            ss << "El dispositivo " << id << " recibe indicacion de que no hay lugar en el sistema de testeo. Reintentara luego";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");
            // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
            usleep(rand() % 10000 + 10000);
            continue;
        }
    
        ss << "El dispositivo " << id << " recibe el programa numero " << program;
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");

        usleep( rand() % 1000 + 1000);

        ss << "El dispositivo " << id << " envia los resultados";
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");

        // Le envio resultado del primer programa de testeo
        int resul = rand() % 10;
        if (resul >= 4){
            atendedor.enviarResultado(id, SEGUIR_TESTEANDO);
        }else if (resul >= 2){
            atendedor.enviarResultado(id, RESULTADO_GRAVE);
        }else{
            atendedor.enviarResultado(id, RESULTADO_NO_GRAVE);
        }
        
        ss << "El dispositivo " << id << " espera la orden del sistema de testeo...";
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");

        // Recibo la orden a seguir
        int orden = atendedor.recibirOrden(id);
        
        if (orden == ORDEN_APAGADO) {
            ss << "El dispositivo " << id << " recibe la orden de apagado (" << orden << "). Byebye!";
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");
            break;
        } else if (orden == ORDEN_REINICIO){
            ss << "El dispositivo " << id << " recibe la orden de reinicio (" << orden << "). Vuelvo pronto!";
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");
            break;
        }else{
            ss << "El dispositivo " << id << " recibe la orden de hacer testeos especiales!";
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");

            bool lastSpecialTest = false;
            program = atendedor.recibirProgramaEspecial(id);
            while (!lastSpecialTest){                
                ss << "El dispositivo " << id << " recibe el programa especial numero " << program << ". Ejecutando test especial...";
                correrTestEspecial( planillaVars );
                Logger::debug(ss.str().c_str(), __FILE__);;
                ss.str("");
                atendedor.enviarResultadoEspecial(id, rand() % 2);
                program = atendedor.recibirProgramaEspecial(id);                
                if (program == FIN_TEST_ESPECIAL)
                    lastSpecialTest = true;
            }
            orden = atendedor.recibirOrden(id);

            if (orden == ORDEN_APAGADO) {
				ss << "El dispositivo " << id << " recibe la orden de apagado (" << orden << "). Byebye!";
				Logger::notice(ss.str().c_str(), __FILE__);
				ss.str("");
				break;
			} else if (orden == ORDEN_REINICIO){
				ss << "El dispositivo " << id << " recibe la orden de reinicio (" << orden << "). Vuelvo pronto!";
				Logger::notice(ss.str().c_str(), __FILE__);
				ss.str("");
				break;
			} else {
				ss << "El dispositivo " << id << " recibe orden final desconocida!!";
				Logger::notice(ss.str().c_str(), __FILE__);
				ss.str("");
				break;
			}
		}
	} catch(std::string exception) {
		Logger::error("Error en el dispositivo...", __FILE__);
		break;
    }
    }
   
    ss << "El dispositivo " << id << " ha terminado el testeo"; 
    Logger::notice(ss.str().c_str(), __FILE__);
    
    Logger::destroy();
    
    return 0;
}

