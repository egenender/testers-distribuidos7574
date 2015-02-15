/* 
 * File:   PlanillaAsignacionEquipoEspecial.cpp
 * Author: ferno
 * 
 * Created on November 16, 2014, 7:56 PM
 */

#include "PlanillaAsignacionEquipoEspecial.h"

PlanillaAsignacionEquipoEspecial::PlanillaAsignacionEquipoEspecial() {

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
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS);
	this->shmemMsgqueueReq = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueReq == -1) {
        Logger::error("Error al construir la msgqueue para requerimientos de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    // Creo la comunicacion al broker para la memoria compartida distribuida
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

	sprintf(paramIdCola, "%d", MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    sprintf(paramId, "%d", ID_EQUIPO_ESPECIAL);
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
/*
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
*/
}

PlanillaAsignacionEquipoEspecial::~PlanillaAsignacionEquipoEspecial() {
}

void PlanillaAsignacionEquipoEspecial::registrarTareaEspecialFinalizada(int idDispositivo) {
    this->obtenerMemoriaCompartida();
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas += 1;
    this->devolverMemoriaCompartida();
}

bool PlanillaAsignacionEquipoEspecial::terminoTesteoEspecial(int posDispositivo, int idDispositivo) {
    this->obtenerMemoriaCompartida();
    int cantTareasPendientes = this->memoria.cantTareasEspecialesAsignadas[posDispositivo].cantTareasEspecialesTotal - this->memoria.cantTareasEspecialesAsignadas[posDispositivo].cantTareasEspecialesTerminadas;
    int cantTestersPendientes = this->memoria.cantTestersEspecialesAsignados[posDispositivo].cantTestersEspecialesTotal - this->memoria.cantTestersEspecialesAsignados[posDispositivo].cantTestersEspecialesTerminados;
    this->devolverMemoriaCompartida();
    std::stringstream ss;
    ss << "Para dispositivo " << idDispositivo << " con posicion " << posDispositivo << ". Cant Tareas pendientes: " << cantTareasPendientes << " y cant testers pendientes: " << cantTestersPendientes;
    Logger::warn(ss.str(), __FILE__);
    return ((cantTareasPendientes == 0) && (cantTestersPendientes == 0));
}

void PlanillaAsignacionEquipoEspecial::reiniciarContadoresTesteoEspecial(int idDispositivo) {
    this->obtenerMemoriaCompartida();
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas = 0;
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal = 0;
    this->memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados = 0;
    this->devolverMemoriaCompartida();
}

void PlanillaAsignacionEquipoEspecial::limpiarContadoresFinTesteo(int idDispositivo) {
    this->obtenerMemoriaCompartida();
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal = 0;
    this->memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas = 0;
    this->memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTotal = 0;
    this->memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados = 0;
    this->devolverMemoriaCompartida(); 
}

void PlanillaAsignacionEquipoEspecial::obtenerMemoriaCompartida() {
    TRequerimientoSharedMemory req;
    req.mtype = MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION;
    req.idSolicitante = ID_EQUIPO_ESPECIAL;
    std::stringstream log; log << "Equipo Especial " << ID_EQUIPO_ESPECIAL << " pide la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    int okSend = msgsnd(this->shmemMsgqueueReq, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el equipo especial " << ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << ID_EQUIPO_ESPECIAL << ". Espero la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
    // Espero por la shmem
    int okRead = msgrcv(this->shmemMsgqueueReceptor, &this->memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), ID_EQUIPO_ESPECIAL, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el equipo especial " << ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << ID_EQUIPO_ESPECIAL << ". Me llega la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
}

void PlanillaAsignacionEquipoEspecial::devolverMemoriaCompartida() {
    std::stringstream log; log << "Equipo Especial " << ID_EQUIPO_ESPECIAL << " devuelve la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    this->memoria.mtype = MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION;
    int okSend = msgsnd(this->shmemMsgqueueEmisor, &this->memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el equipo especial " << ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << ID_EQUIPO_ESPECIAL << " devuelta la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
}