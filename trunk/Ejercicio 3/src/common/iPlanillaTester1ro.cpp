#include "iPlanillaTester1ro.h"
#include "Configuracion.h"
#include "common.h"
#include <sys/msg.h>
#include <sys/ipc.h>

iPlanillaTester1ro::iPlanillaTester1ro(int id, const Configuracion& config) {
    idTester = id;
    key_t key = ftok( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_PLANILLA) );
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTester1ro::~iPlanillaTester1ro() {
}


bool iPlanillaTester1ro::agregar(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_AGREGAR;
    requerimiento.idDispositivo = idDispositivo;

    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);

    respuesta_lugar_t hayLugar;
    msgrcv(cola, &hayLugar, sizeof(respuesta_lugar_t) - sizeof(long),idDispositivo ,0);
    return hayLugar.respuesta;
}

void iPlanillaTester1ro::terminoRequerimientoPendiente(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_TERMINO_PENDIENTE_REQ;
    requerimiento.idDispositivo = idDispositivo;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

