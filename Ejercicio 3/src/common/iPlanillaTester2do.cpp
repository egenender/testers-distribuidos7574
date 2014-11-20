#include "iPlanillaTester2do.h"
#include "Configuracion.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/msg.h>

iPlanillaTester2do::iPlanillaTester2do(int id, const Configuracion& config) {
    idTester = id;
    key_t key = ftok( config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_PLANILLA) );
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTester2do::iPlanillaTester2do(const iPlanillaTester2do& orig) {
}

iPlanillaTester2do::~iPlanillaTester2do() {
}

void iPlanillaTester2do::iniciarProcesamientoDeResultados(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_PROCESAR_SIGUIENTE;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

void iPlanillaTester2do::procesarSiguiente(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_PROCESAR_SIGUIENTE;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}


