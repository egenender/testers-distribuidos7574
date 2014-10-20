#include "iPlanillaTester1ro.h"
#include "common.h"
#include <sys/ipc.h>

iPlanillaTester2do::iPlanillaTester2do(int id) {
    idTester = id;
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA);
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTester2do::iPlanillaTester2do(const iPlanillaTester2do& orig) {
}

iPlanillaTester2do::~iPlanillaTester2do() {
}
void iPlanillaTesterRespuesta::iniciarProcesamientoDeResultados(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_PROCESAR_SIGUIENTE;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

void iPlanillaTesterRespuesta::procesarSiguiente(){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_PROCESAR_SIGUIENTE;
    requerimiento.idDispositivo = 0;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}


