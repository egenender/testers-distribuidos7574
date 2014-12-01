#include <cstdlib>

#include "common/AtendedorTesters.h"
#include "common/Programa.h"
#include "common/common.h"
#include "common/Resultado.h"
#include "common/iPlanillaTesterRespuesta.h"
#include "common/Configuracion.h"
#include "common/DespachadorTesters.h"
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
    int id = config.ObtenerParametroEntero("TesterIdOffset") + atoi(argv[1]);


    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor(config);
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterRespuesta planilla(id,config);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador(config);

    while (true) {
        
        
        planilla.iniciarProcesamientoDeResultados();
        resultado_test_t resultado = atendedor.recibirResultado(id);

        int orden;
        if (Resultado::esGrave(resultado.result)) {
            despachador.enviarOrden(resultado.dispositivo);
            orden = Constantes::ORDEN_APAGADO;
        } else {
            orden = Constantes::ORDEN_REINICIO;
        }
        atendedor.enviarOrden(resultado.dispositivo, orden);

        planilla.eliminarDispositivo(resultado.dispositivo);
        planilla.procesarSiguiente();
    }

    return 0;
}
