/*
 * File:   terminador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 *
 * Destruye todos los IPCs utilizados en la aplicacion
 */

#include "logger/Logger.h"
#include "common/AtendedorTesters.h"
#include "common/DespachadorTecnicos.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Configuracion.h"
#include "common/common.h"
#include <cstdlib>
#include <cerrno>
#include <cstring>

int main( int argc, char** argv ){

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    Logger::notice("Logger inicializado. Destruyendo IPCs...", __FILE__);
   
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    // Creo objeto atendedor para destruir sus ipcs
    AtendedorTesters atendedor( config );
    if( !atendedor.destruirComunicacion() ) {
        Logger::error("No se pudo destruir la cola de mensajes del atendedor...", __FILE__);
    }

    // Creo objeto despachador para destruir sus ipcs
    DespachadorTecnicos despachador( config );
    if( !despachador.destruirComunicacion() ) {
        Logger::error("No se pudo destruir la cola de mensajes del despachador...", __FILE__);
    }

    //Shared memory general de planilla: la destruyo a traves de una instancia asociada al primer tester
    const int iTesterStart = config.ObtenerParametroEntero( Constantes::NombresDeParametros::ID_TESTER_START );
    Planilla planilla( iTesterStart, config );
    if( !planilla.destruirMemoriaGeneral() ){
        Logger::error("No se pudo destruir la shared memory general de la planilla...", __FILE__);
    }
    //Cola planilla: idem
    if( !planilla.destruirCola() ){
        Logger::error("No se pudo destruir la cola general de la planilla...", __FILE__);
    }
    //Mutex general: idem
    if( !planilla.destruirSemaforoGeneral() ){
        Logger::error("No se pudo destruir el semaforo general de la planilla...", __FILE__);
    }

    //Semaforos y shms locales de planillas
    const int cantTesters = config.ObtenerParametroEntero( Constantes::NombresDeParametros::CANT_TESTERS );
    for( int iTester=iTesterStart; iTester<(iTesterStart+cantTesters); iTester++ ){
        Planilla planilla( iTester, config );
        if (!planilla.destruirMemoriaLocal()) {
            Logger::error("No se pudo destruir la memoria compartida de la planilla...", __FILE__);
        }
        std::string msjError;
        if( !planilla.destruirSemaforosLocales( msjError ) ){
            Logger::error(msjError, __FILE__);
        }
    }

    unlink( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ).c_str() );

    Logger::notice("IPCs eliminados...", __FILE__);

    Logger::destroy();

    return 0;
}
