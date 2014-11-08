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

int ordenAEnviar(){
	int resul = rand() % 10;
    if (resul >= 4){
		return SEGUIR_TESTEANDO;
	}else if (resul >= 2){
		return RESULTADO_GRAVE;
	}else{
		return RESULTADO_NO_GRAVE;
	}
}

void seguirTesteando(int id, AtendedorDispositivos* atendedor,int cantidad){
	std::stringstream ss;
	for (int i = 0; i < cantidad; i++){
		ss << "El dispositivo " << id << " espera por un programa especial";
		Logger::debug(ss.str().c_str(), __FILE__);;
		ss.str("");
		int program = atendedor->recibirPrograma(id);
		usleep(rand() % 1000 + 1000);
		ss << "El dispositivo " << id << " recibe el programa especial numero " << program << ". Enviando resultados...";
		Logger::debug(ss.str().c_str(), __FILE__);;
		ss.str("");
		atendedor->enviarResultado(id, rand() % 2);
	}
	int aux; //no deberia cambiar la cantidad
	int orden = atendedor->recibirOrden(id, &aux);
		
	if (orden == ORDEN_APAGADO) {
		ss << "El dispositivo " << id << " recibe la orden de apagado (" << orden << "). Byebye!";
		Logger::notice(ss.str().c_str(), __FILE__);
		ss.str("");
	} else if (orden == ORDEN_REINICIO){
		ss << "El dispositivo " << id << " recibe la orden de reinicio (" << orden << "). Vuelvo pronto!";
		Logger::notice(ss.str().c_str(), __FILE__);
		ss.str("");
	} else {
		//Debo seguir testeando en este ejercicio
		ss << "El dispositivo " << id << " recibe la orden de seguir testeando (seguramente se encontro algun error)";
		Logger::notice(ss.str().c_str(), __FILE__);
		ss.str("");
		seguirTesteando(id, atendedor, cantidad);
	}
}

int main(int argc, char** argv) {
	srand(getpid());
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    // Por parametro se recibe el ID del dispositivo
    int id = atoi(argv[1]);

    std::stringstream ss;
    ss << "El dispositivo " << id << " se crea";
    Logger::debug(ss.str().c_str(), __FILE__);
    ss.str("");
    
    // Comunicacion con el sistema de testeo
    AtendedorDispositivos atendedor;
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
            sleep(rand() % 10000 + 10000);
            continue;
        }
    
		ss << "El dispositivo " << id << " recibe el programa numero " << program << ". Enviando resultados...";
		Logger::debug(ss.str().c_str(), __FILE__);;
		ss.str("");
		usleep(rand() % 1000 + 1000);
        // Le envio resultado del primer programa de testeo
        atendedor.enviarResultado(id, ordenAEnviar());
                     
		ss << "El dispositivo " << id << " espera la orden del sistema de testeo...";
		Logger::debug(ss.str().c_str(), __FILE__);
		ss.str("");

        // Recibo la orden a seguir
        int cantidad;
        int orden = atendedor.recibirOrden(id, &cantidad);
        
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
			
			seguirTesteando(id, &atendedor, cantidad);
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

