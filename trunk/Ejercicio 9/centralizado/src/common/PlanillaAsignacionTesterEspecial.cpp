/* 
 * File:   PlanillaAsignacionTesterEspecial.cpp
 * Author: ferno
 * 
 * Created on November 16, 2014, 7:56 PM
 */

#include "PlanillaAsignacionTesterEspecial.h"

PlanillaAsignacionTesterEspecial::PlanillaAsignacionTesterEspecial() : semShmemCantTesters(SEM_PLANILLA_CANT_TESTER_ASIGNADOS), semShmemCantTareas(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS) {
    this->shmemCantTestersKey = ftok(ipcFileName.c_str(), SHM_PLANILLA_CANT_TESTER_ASIGNADOS);
    if(this->shmemCantTestersKey == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shmemCantTestersId = shmget(this->shmemCantTestersKey, sizeof(TContadorTesterEspecial) * MAX_DISPOSITIVOS_EN_SISTEMA, IPC_CREAT | 0660);
    if(this->shmemCantTestersId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    void* tmpPtr = shmat (this->shmemCantTestersId , NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        this->cantTestersEspecialesAsignados = static_cast<TContadorTesterEspecial*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla de asignacion de testers creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla de asignacion de testers. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if (!this->semShmemCantTesters.getSem()) {
	std::string err = std::string("Error al obtener el semaforo de la planilla de asignacion de testers. Error: ") + std::string(strerror(errno));
	Logger::error(err, __FILE__);
	throw err;
    }
    
    this->shmemCantTareasKey = ftok(ipcFileName.c_str(), SHM_PLANILLA_CANT_TAREAS_ASIGNADAS);
    if(this->shmemCantTareasKey == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla de asignacion de tareas. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shmemCantTareasId = shmget(this->shmemCantTareasKey, sizeof(TContadorTareaEspecial) * MAX_DISPOSITIVOS_EN_SISTEMA, IPC_CREAT | 0660);
    if(this->shmemCantTareasId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla de asignacion de tareas. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    tmpPtr = shmat (this->shmemCantTareasId , NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        this->cantTareasEspecialesAsignadas = static_cast<TContadorTareaEspecial*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla de asignacion de tareas creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla de asignacion de tareas. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if (!this->semShmemCantTareas.getSem()) {
	std::string err = std::string("Error al obtener el semaforo de la planilla de asignacion de tareas. Error: ") + std::string(strerror(errno));
	Logger::error(err, __FILE__);
	throw err;
    }
}

void PlanillaAsignacionTesterEspecial::asignarCantTareasEspeciales(int idDispositivo, int cantTareas) {
    this->semShmemCantTareas.p();
    this->cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal += cantTareas;
    this->semShmemCantTareas.v();
}
void PlanillaAsignacionTesterEspecial::avisarFinEnvioTareas(int idDispositivo) {
    this->semShmemCantTesters.p();
    this->cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados += 1;
    this->semShmemCantTesters.v();
}

PlanillaAsignacionTesterEspecial::~PlanillaAsignacionTesterEspecial() {
}

bool PlanillaAsignacionTesterEspecial::destruirComunicacion() {
    return (!((shmctl(this->shmemCantTestersId, IPC_RMID, NULL) != -1) || (shmctl(this->shmemCantTareasId, IPC_RMID, NULL) != -1)));
}