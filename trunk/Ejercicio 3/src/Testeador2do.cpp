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
    iPlanillaTester2do planilla(id,config);

    while (true) {


        // Espero un requerimiento
        int idDispositivo = atendedor.recibir2doRequerimiento(id);

        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());

        planilla.iniciarProcesamientoDeResultadosParciales();

        planilla.procesarSiguiente();


    }

    return 0;
}

