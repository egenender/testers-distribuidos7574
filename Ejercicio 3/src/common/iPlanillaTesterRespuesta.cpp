#include "iPlanillaTesterRespuesta.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/msg.h>

iPlanillaTesterRespuesta::iPlanillaTesterRespuesta(int id) {
    idTester = id;
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA);
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTesterRespuesta::iPlanillaTesterRespuesta(const iPlanillaTesterRespuesta& orig) {
}

iPlanillaTesterRespuesta::~iPlanillaTesterRespuesta() {
}

void iPlanillaTesterRespuesta::eliminarDispositivo(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_ELIMINAR_DISPOSITIVO;
    requerimiento.idDispositivo = idDispositivo;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
    //Deberia esperar un ok?
}

void iPlanillaTesterRespuesta::iniciarProcesamientoDeResultados(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_ELIMINAR_DISPOSITIVO;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

void iPlanillaTesterRespuesta::procesarSiguiente(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_ELIMINAR_DISPOSITIVO;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}



