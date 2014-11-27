#include "iPlanillaTesterRespuesta.h"
#include "Configuracion.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/msg.h>

iPlanillaTesterRespuesta::iPlanillaTesterRespuesta(int id, const Configuracion& config) {
    idTester = id;
    key_t key = ftok( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ).c_str(),
                      config.ObtenerParametroEntero( Constantes::NombresDeParametros::MSGQUEUE_PLANILLA ));
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTesterRespuesta::iPlanillaTesterRespuesta(const iPlanillaTesterRespuesta& orig) {
}

iPlanillaTesterRespuesta::~iPlanillaTesterRespuesta() {
}

void iPlanillaTesterRespuesta::eliminarDispositivo(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_ELIMINAR_DISPOSITIVO;
    requerimiento.idDispositivo = idDispositivo;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
    //Deberia esperar un ok?
}

void iPlanillaTesterRespuesta::iniciarProcesamientoDeResultados(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_INICIAR_PROC_RESULTADOS;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

void iPlanillaTesterRespuesta::procesarSiguiente(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_PROCESAR_SIGUIENTE;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

void iPlanillaTesterRespuesta::agregarResultado(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = Constantes::REQUERIMIENTO_AGREGAR_RESULTADO;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}



