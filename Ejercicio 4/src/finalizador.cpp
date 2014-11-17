/* 
 * File:   terminador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Destruye todos los IPCs utilizados en la aplicacion
 */

/*
#include "unistd.h"*/

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
    if (!atendedor.destruirComunicacion()) {
        Logger::error("No se pudo destruir la cola de mensajes del atendedor...", __FILE__);
    }

    // Creo objeto despachador para destruir sus ipcs
    DespachadorTecnicos despachador( config );
    if (!despachador.destruirComunicacion()) {
        Logger::error("No se pudo destruir la cola de mensajes del despachador...", __FILE__);
    }

    // Creo objeto planilla para destruir el semaforo y la memoria compartida que utiliza
    Planilla planilla( config );
    if (!planilla.destruirMemoria()) {
        Logger::error("No se pudo destruir la memoria compartida de la planilla...", __FILE__);
    }
    if (!planilla.destruirSemaforo()) {
        std::string err = std::string("No se pudo destruir el semaforo de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
    }

    unlink( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str() );

    Logger::notice("IPCs eliminados...", __FILE__);

    Logger::destroy();

    return 0;
}

