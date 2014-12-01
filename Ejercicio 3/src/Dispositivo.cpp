#include <cstdlib>
#include <sstream>

#include "common/AtendedorDispositivos.h"
#include "common/Configuracion.h"
#include "logger/Logger.h"
#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
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

    // TODO: Log

    while (true) {
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
                sleep(rand() % 5 + 5);
                continue;
            }

            ss << "El dispositivo " << id << " recibe el programa numero " << program << ". Enviando respuesta...";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            usleep(rand() % 1000 + 1000);
            // Le envio resultado del primer programa de testeo
            atendedor.enviar1erRespuesta(id, rand() % 2);

            // Recibe el segundo programa, verificando que no sea un rechazo por parte del sistema

            do {
                program = atendedor.recibirPrograma(id);
                if (program == -1) {
                    ss << "El dispositivo " << id << " recibe indicacion de que no hay lugar en el sistema de testeo. Reintentara luego";
                    Logger::debug(ss.str().c_str(), __FILE__);
                    ss.str("");
                    // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
                    sleep(rand() % 60 + 60);
                    continue;
                }
            } while (program == -1);

            ss << "El dispositivo " << id << " recibe el programa numero " << program << ". Enviando 2da respuesta...";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            usleep(rand() % 1000 + 1000);
            // Le envio resultado del segundo programa de testeo
            atendedor.enviarResultado(id, rand() % 2);

            ss << "El dispositivo " << id << " espera la orden del sistema de testeo...";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");

            // Recibo la orden a seguir
            int orden = atendedor.recibirOrden(id);

            if (orden == Constantes::ORDEN_APAGADO) {
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
        } catch (std::string exception) {
            Logger::error("Error en el dispositivo...", __FILE__);
            break;
        }
    }

    ss << "El dispositivo " << id << " ha terminado el testeo";
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");

    Logger::destroy();

    return 0;
}

