#include <cstdlib>

#include "common/AtendedorTesters.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/iPlanillaTester2do.h"
#include "common/Configuracion.h"
#include "logger/Logger.h"



using namespace std;

int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    // El primer parametro es el id del tester
    int id = config.ObtenerParametroEntero("Tester2doIdOffset") + atoi(argv[1]);


    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor(config);
    // Obtengo planilla general de sync con otros tester
    iPlanillaTester2do planilla(id, config);

    std::stringstream ss;
    
    while (true) {


        // Espero un requerimiento
        int idDispositivo = atendedor.recibir2doRequerimiento(id);

        ss << "Se recibio un 2do Requerimiento del dispositivo " << idDispositivo ;
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");

        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());

        Logger::notice("Se envio el 2do programa", __FILE__);

        planilla.iniciarProcesamientoDeResultadosParciales();

        Logger::notice("Termino el procesamiento de resultados parciales", __FILE__);

        planilla.procesarSiguiente();

        Logger::notice("Termino procesar siguiente", __FILE__);

    }

    return 0;
}

