#include "iPlanillaTester1ro.h"
#include "Configuracion.h"
#include "common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "logger/Logger.h"

iPlanillaTester1ro::iPlanillaTester1ro(int id, const Configuracion& config) {
    idTester = id;
    key_t key = ftok(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
            config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_PLANILLA));
    this->cola = msgget(key, 0666 | IPC_CREAT);

    std::stringstream ss; //<DBG>
    ss << "MSGQUEUE_PLANILLA creada con id " << cola;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");
}

iPlanillaTester1ro::~iPlanillaTester1ro() {
}

bool iPlanillaTester1ro::agregar(int idDispositivo) {
    requerimiento_planilla_t requerimiento;
    requerimiento.mtype = Constantes::MTYPE_REQUERIMIENTO;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_AGREGAR;
    requerimiento.idDispositivo = idDispositivo;

    if (-1 == msgsnd(cola, &requerimiento, sizeof (requerimiento_planilla_t) - sizeof (long), 0)) {

        std::string error = std::string("Error al hacer msgsnd. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(-1);

    }

    Logger::notice("se envio un nuevo requerimiento", __FILE__);

    respuesta_lugar_t hayLugar;
    if (-1 == msgrcv(cola, &hayLugar, sizeof (respuesta_lugar_t) - sizeof (long), idDispositivo + Constantes::OFFSET_LUGAR, 0)) {

        std::string error = std::string("Error al hacer msgrcv. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(-1);

    }

    return hayLugar.respuesta;
}

void iPlanillaTester1ro::terminoRequerimientoPendiente(int idDispositivo) {
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_TERMINO_PENDIENTE_REQ;
    requerimiento.idDispositivo = idDispositivo;

    if (-1 == msgsnd(cola, &requerimiento, sizeof (requerimiento_planilla_t) - sizeof (long), 0)) {


        std::string error = std::string("Error al hacer msgsnd. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(-1);

    }
}

