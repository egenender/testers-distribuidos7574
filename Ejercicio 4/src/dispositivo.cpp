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
#include "logger/Logger.h"
#include "common/common.h"
#include <sstream>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char** argv) { //TODO Esto esta muy chorizo, dividir en metodos

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    // Se recibe el ID del dispositivo por parametro
    int id = atoi(argv[1]);

    std::stringstream ss;
    ss << "Dispositivo " << id << " creado";
    Logger::debug(ss.str().c_str(), __FILE__);
    ss.str("");

    //Parametros del sistema
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error( "Archivo de configuracion no encontrado", __FILE__ );
        return 1;
    }
    const int minTiempoEsperaReintento = config.ObtenerParametroEntero( Constantes::NombresDeParametros::MIN_TIEMPO_ESPERA_REINTENTO_TESTER_OCUPADO );
    const int maxTiempoEsperaReintento = config.ObtenerParametroEntero( Constantes::NombresDeParametros::MAX_TIEMPO_ESPERA_REINTENTO_TESTER_OCUPADO );

    // Comunicacion con el sistema de testeo
    AtendedorDispositivos atendedor( config );

    //Generar lista con ids de testers para ir eligiendolos al azar
    std::vector<int> idsTesters;
    for( int iTester=1; iTester<= config.ObtenerParametroEntero( Constantes::NombresDeParametros::CANT_TESTERS_ESP ); iTester++ )
        idsTesters.push_back( iTester );
    bool apagar = false;

    while(true) {
        try {
            //Seleccionar tester especial al azar
            int idTester = idsTesters[ rand() % idsTesters.size() ];
            idsTesters.erase( std::remove(idsTesters.begin(), idsTesters.end(), idTester), idsTesters.end() );

            // Dispositivo envia requerimiento
            ss << "El dispositivo " << id << " enviará un requerimiento al tester especial " << idTester;
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");
            atendedor.enviarRequerimiento( idTester, id);

            // Recibe programa, verificando que no sea un rechazo por parte del sistema
            int program = atendedor.recibirPrograma(id);
            if (program == -1) {
                ss << "El dispositivo " << id << " recibió la indicacion de que no hay lugar en el sistema de testing. Reintentará luego";
                Logger::debug(ss.str().c_str(), __FILE__);
                ss.str("");
                // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
                sleep( minTiempoEsperaReintento + rand() % ( maxTiempoEsperaReintento - minTiempoEsperaReintento? maxTiempoEsperaReintento - minTiempoEsperaReintento : 1 ) );
                continue;
            }

            ss << "El dispositivo " << id << " recibió el programa numero " << program << ". Enviando resultados...";
            Logger::debug(ss.str().c_str(), __FILE__);;
            ss.str("");

            // Envio resultado del programa de test
            atendedor.enviarResultado(id, rand() % 2);

            // Recibo la orden a seguir y la proceso
            ss << "El dispositivo " << id << " esperará la orden del sistema de testeo...";
            Logger::debug(ss.str().c_str(), __FILE__);
            ss.str("");
            int orden = atendedor.recibirOrden(id);

            if (orden == Constantes::ORDEN_APAGADO) {
                ss << "El dispositivo " << id << " recibió la orden de apagado (" << orden << "). del tester esp " << idTester;
                Logger::notice(ss.str().c_str(), __FILE__);
                ss.str("");
                //break;
            } else {
                ss << "El dispositivo " << id << " recibió la orden de reinicio (" << orden << ") del tester esp " << idTester;
                Logger::notice(ss.str().c_str(), __FILE__);
                ss.str("");
                //break;
            }

            //Si ya no quedan testers que consultar, decidir si apagar o no y terminar
            if( idsTesters.empty() ){
                if(apagar){
                    ss << "El dispositivo " << id << " recibió al menos una orden de apagado. Bye bye!";
                    Logger::notice(ss.str().c_str(), __FILE__);
                    ss.str("");
                }else{
                    ss << "El dispositivo " << id << " no recibio ninguna orden de apagado. ¡Volverá pronto!";
                    Logger::notice(ss.str().c_str(), __FILE__);
                    ss.str("");
                }
                break;
            }
        } catch(std::string exception) {
            Logger::error("Error en el dispositivo: " + exception, __FILE__);
            break;
        }
    }

    ss << "El dispositivo " << id << " ha terminado el testeo";
    Logger::notice(ss.str().c_str(), __FILE__);

    Logger::destroy();

    return 0;
}

