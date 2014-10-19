#include "iPlanillaTesterA.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

iPlanillaTesterA::iPlanillaTesterA(int id) {
    idTester = id;
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA);
    this->cola = msgget(key, IPC_CREAT);
}

iPlanillaTesterA::iPlanillaTesterA(const iPlanillaTesterA& orig) {
}

iPlanillaTesterA::~iPlanillaTesterA() {
}


bool iPlanillaTesterA::agregar(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_AGREGAR;
    requerimiento.idDispositivo = idDispositivo;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
    
    respuesta_lugar_t hayLugar;
    msgrcv(cola, &hayLugar, sizeof(respuesta_lugar_t) - sizeof(long),idDispositivo ,0);
    return hayLugar.respuesta;
}

void iPlanillaTesterA::terminoRequerimientoPendiente(int idDispositivo){
    requerimiento_planilla_t requerimiento;
    requerimiento.tester = idTester;
    requerimiento.tipoReq = REQUERIMIENTO_TERMINO_PENDIENTE_REQ;
    requerimiento.idDispositivo = idDispositivo;
    
    msgsnd(cola,&requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long),0);
}

