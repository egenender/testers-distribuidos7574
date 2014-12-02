#include <cstdlib>
#include "common/common.h"
#include "common/Planilla.h"
#include "common/Configuracion.h"
#include <sstream>
#include <cerrno>
#include <cstring>
#include "logger/Logger.h"


using namespace Constantes;
using namespace std;

int main(int argc, char** argv) {
    int id = atoi(argv[1]);

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << "Inicia el procesamiento, cargando IPCS id " << id;
    Logger::notice( nombre.str().c_str(),__FILE__);

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    Planilla planilla(id, config);
    requerimiento_planilla_t requerimiento;

    std::stringstream ss;

    Logger::notice("Entrando a loop principal", __FILE__);
    while (true) {
        if (-1 == msgrcv(planilla.queue(), &requerimiento, sizeof (requerimiento_planilla_t) - sizeof (long), Constantes::MTYPE_REQUERIMIENTO, 0)) {

            std::string error = std::string("Error al hacer msgrcv. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(-1);

        }
        switch (requerimiento.tipoReq) {
            case Constantes::REQUERIMIENTO_AGREGAR:
                ss << "Recibo un pedido para agregar el dispositivo " << requerimiento.idDispositivo << " a la planilla";
                Logger::notice(ss.str().c_str(), __FILE__);
                ss.str("");
                planilla.agregar(requerimiento.idDispositivo);
                break;
            case Constantes::REQUERIMIENTO_TERMINO_PENDIENTE_REQ:
                Logger::notice("Recibo un pedido para marcar la finalizacion de requerimiento pendiente", __FILE__);
                planilla.terminadoRequerimientoPendiente();
                break;
        }

    }

    return 0;
}

