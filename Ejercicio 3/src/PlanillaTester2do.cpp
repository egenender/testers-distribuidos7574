#include <cstdlib>
#include "common/common.h"
#include "common/Planilla.h"
#include "common/Configuracion.h"
#include <cerrno>
#include <cstring>
#include "logger/Logger.h"


using namespace Constantes;
using namespace std;

int main(int argc, char** argv) {

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__;
    Logger::notice("Inicia el procesamiento, cargando IPCS", nombre.str().c_str());

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    int id = config.ObtenerParametroEntero("Tester2doIdOffset") + atoi(argv[1]);
    
    Planilla planilla(id, config);
    requerimiento_planilla_t requerimiento;

    Logger::notice("Entrando a loop principal", nombre.str().c_str());
    while (true) {
        if (-1 == msgrcv(planilla.queue(), &requerimiento, sizeof (requerimiento_planilla_t) - sizeof (long), id, 0)) {
            std::string error = std::string("Error al hacer msgrcv. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(-1);
        }
        switch (requerimiento.tipoReq) {
            case REQUERIMIENTO_INICIAR_PROC_RESULTADOS_PARCIALES:
                Logger::notice("Recibo un pedido para iniciar el procesamiento de resultados parciales", nombre.str().c_str());
                planilla.iniciarProcesamientoResultadosParciales();
                break;
            case REQUERIMIENTO_PROCESAR_SIGUIENTE:
                Logger::notice("Recibo un pedido para procesar siguiente", nombre.str().c_str());
                planilla.procesarSiguiente();
                break;
            case REQUERIMIENTO_AGREGAR_RESULTADO_PARCIAL:
                Logger::notice("Recibo un pedido para agregar un resultado parcial", nombre.str().c_str());
                planilla.agregarResultadoParcial();
                break;
        }

    }

    return 0;
}

