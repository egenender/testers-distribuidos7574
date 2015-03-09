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
#include "common/Configuracion.h"

using namespace std;

int getIdDispositivo(); // Request al server RPC de identificadores

int main(int argc, char* argv[]) {

    srand(time(NULL));
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    // Por parametro se recibe el ID del dispositivo
    int id = getIdDispositivo();
    
    if (id == 0) {
        Logger::error("No hay ID disponible para este dispositivo", __FILE__);
        exit(1);
    }

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
    AtendedorDispositivos atendedor( id, config );
    
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
            Logger::debug(ss.str().c_str(), __FILE__);;
            ss.str("");

            usleep( rand() % 1000 + 1000);

            ss << "El dispositivo " << id << " envia los resultados";
            Logger::debug(ss.str().c_str(), __FILE__);;
            ss.str("");

            // Le envio resultado del primer programa de testeo
            int resul = rand() % 10;
            if (resul >= 4) {
                atendedor.enviarResultado( id, Constantes::SEGUIR_TESTEANDO );
            } else if (resul >= 2) {
                atendedor.enviarResultado( id, Constantes::RESULTADO_GRAVE );
            } else {
                atendedor.enviarResultado( id, Constantes::RESULTADO_NO_GRAVE );
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
            } else {
                ss << "El dispositivo " << id << " recibe la orden de hacer testeos especiales!";
                Logger::notice(ss.str().c_str(), __FILE__);
                ss.str("");

                bool lastSpecialTest = false;
                program = atendedor.recibirProgramaEspecial(id);
                while (!lastSpecialTest){
                    ss << "El dispositivo " << id << " recibe el programa especial numero " << program << ". Enviando resultados...";
                    Logger::debug(ss.str().c_str(), __FILE__);;
                    ss.str("");
                    atendedor.enviarResultadoEspecial(id, rand() % 2);
                    program = atendedor.recibirProgramaEspecial(id);
                    if ( program == Constantes::FIN_TEST_ESPECIAL )
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

int getIdDispositivo() {
    
    CLIENT *clnt;
    int  *result_1;
    char *getiddispositivo_1_arg;
    
    clnt = clnt_create (UBICACION_SERVER_IDENTIFICADOR, IDENTIFICADORPROG, IDENTIFICADORVERS, "udp");
    if (clnt == NULL) {
        // TODO: Log
        clnt_pcreateerror (UBICACION_SERVER_IDENTIFICADOR);
        exit (1);
    }
    
    result_1 = getiddispositivo_1((void*)&getiddispositivo_1_arg, clnt);
    if (result_1 == (int *) NULL) {
        // TODO: Log
        clnt_perror (clnt, "call failed");
    }
    
    clnt_destroy (clnt);
    
    return *result_1;
}
