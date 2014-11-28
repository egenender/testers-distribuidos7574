#include <cstdlib>

#include "common/AtendedorTesters.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/iPlanillaTester1ro.h"
#include "common/DespachadorTecnicos.h"
#include "common/Configuracion.h"
#include "logger/Logger.h"

using namespace std;

int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor( config );
    // Obtengo planilla general de sync con otros tester
    iPlanillaTester1ro planilla(id, config);

    while(true) {

        // Espero un requerimiento
        int idDispositivo = atendedor.recibirRequerimiento();

        if( !planilla.agregar(idDispositivo) ) {
            // Si no hay lugar se le avisa con un -1 en vez de programa
            atendedor.enviarPrograma(idDispositivo, id, Constantes::SIN_LUGAR);
            continue;
        }

        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        planilla.terminoRequerimientoPendiente( idDispositivo );
    }

    return 0;
}

