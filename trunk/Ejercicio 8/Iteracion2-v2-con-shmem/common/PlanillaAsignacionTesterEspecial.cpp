/* 
 * File:   PlanillaAsignacionTesterEspecial.cpp
 * Author: ferno
 * 
 * Created on November 16, 2014, 7:56 PM
 */

#include "PlanillaAsignacionTesterEspecial.h"

PlanillaAsignacionTesterEspecial::PlanillaAsignacionTesterEspecial(int idTester) : idTester(idTester) {
    // Obtengo la msgqueue de envio y recepcion de shmem
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION);
	this->shmemMsgqueueEmisor = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueEmisor == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
	this->shmemMsgqueueReceptor = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueReceptor == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    // Creo la comunicacion al broker para la memoria compartida distribuida
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

	sprintf(paramIdCola, "%d", MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    sprintf(paramId, "%d", this->idTester);
    sprintf(paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));

	this->pidReceptor = fork();
	if (this->pidReceptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION,
                paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
		exit(1);
	}

    sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes
    this->pidEmisor = fork();
	if (this->pidEmisor == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION,
				paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}
}

void PlanillaAsignacionTesterEspecial::asignarCantTareasEspeciales(int idDispositivo, int cantTareas) {
    this->obtenerMemoriaCompartida();
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal += cantTareas;
    this->devolverMemoriaCompartida();
}
void PlanillaAsignacionTesterEspecial::avisarFinEnvioTareas(int idDispositivo) {
    this->obtenerMemoriaCompartida();
    this->memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados += 1;
    this->devolverMemoriaCompartida();
}

PlanillaAsignacionTesterEspecial::~PlanillaAsignacionTesterEspecial() {
}

void PlanillaAsignacionTesterEspecial::obtenerMemoriaCompartida() {
    this->memoria.mtype = MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION;
    this->memoria.idSolicitante = this->idTester;
    int okSend = msgsnd(this->shmemMsgqueueEmisor, &this->memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el tester especial " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    // Espero por la shmem
    int okRead = msgrcv(this->shmemMsgqueueReceptor, &this->memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), this->idTester, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el tester especial " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
}

void PlanillaAsignacionTesterEspecial::devolverMemoriaCompartida() {
    this->memoria.mtype = MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION;
    int okSend = msgsnd(this->shmemMsgqueueEmisor, &this->memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el tester especial " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
}