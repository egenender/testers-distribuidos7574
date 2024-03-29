/* 
 * File:   Dispositivo.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:12 PM
 * 
 * El dispositivo interactua con el sistema de testeo, enviando requerimientos, recibiendo programas, enviando resultados
 * y esperando ordenes
 */

#include <cstdlib>
#include <sstream>

#include "common/AtendedorDispositivos.h"
#include "logger/Logger.h"
#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
	srand(time(NULL));
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    // Por parametro se recibe el ID del dispositivo
    int id = atoi(argv[1]);

    std::stringstream ss;
    ss << "El dispositivo " << id << " se crea";
    Logger::debug(ss.str().c_str(), __FILE__);
    ss.str("");
    
    // Comunicacion con el sistema de testeo
    AtendedorDispositivos atendedor(id);
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
        if (program == -1) {
	    ss << "El dispositivo " << id << " recibe indicacion de que no hay lugar en el sistema de testeo. Reintentara luego";
	    Logger::debug(ss.str().c_str(), __FILE__);
	    ss.str("");
            // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
            sleep(rand() % 60 + 60);
            continue;
        }
    
		ss << "El dispositivo " << id << " recibe el programa numero " << program;
		Logger::debug(ss.str().c_str(), __FILE__);;
		ss.str("");
		
        usleep(rand() % 1000 + 1000);
        ss << "El dispositivo " << id << " envia los resultados";
		Logger::debug(ss.str().c_str(), __FILE__);;
		ss.str("");
		
        atendedor.enviarResultado(id, rand() % 2);
        
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
        } else {
	    ss << "El dispositivo " << id << " recibe la orden de reinicio (" << orden << "). Vuelvo pronto!";
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");
            break;
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

