/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "logger/Logger.h"

Planilla::Planilla() : semShMem(SEM_PLANILLA) {
    
    this->shMemKey = ftok(ipcFileName.c_str(), SHMEM_PLANILLA);
    if(this->shMemKey == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shMemId = shmget(this->shMemKey, sizeof(int), IPC_CREAT | 0666);
    if(this->shMemId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    void* tmpPtr = shmat (this->shMemId , NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        this->cantDispositivosSiendoTesteados = static_cast<int*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if (!this->semShMem.getSem()) {
	std::string err = std::string("Error al obtener el semaforo de la planilla. Error: ") + std::string(strerror(errno));
	Logger::error(err, __FILE__);
	throw err;
    }

}

Planilla::Planilla(const Planilla& orig) : semShMem(SEM_PLANILLA) {
}

Planilla::~Planilla() {
}

bool Planilla::hayLugar() {

    // Si hay lugar, registro el dispositivo
    bool result = true;
    this->semShMem.p();
    if(*this->cantDispositivosSiendoTesteados >= MAX_DISPOSITIVOS_EN_SISTEMA) {
        result = false;
    } else {
        *this->cantDispositivosSiendoTesteados += 1;
    }
    this->semShMem.v();
    return result;
}
void Planilla::eliminarDispositivo() {

    this->semShMem.p();
    *this->cantDispositivosSiendoTesteados -= 1;
    this->semShMem.v();
}

int Planilla::cantProcesosUsandoPlanilla() {
    shmid_ds estado;
    if (shmctl(this->shMemId, IPC_STAT, &estado) == -1) {
        std::string err = std::string("Error en shmctl() al verificar la cantidad de procesos adosados a la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
    }
	return estado.shm_nattch;
}

bool Planilla::destruirMemoria() {

    return (shmctl(this->shMemId, IPC_RMID, NULL) != -1);
}

bool Planilla::destruirSemaforo() {

    return (this->semShMem.eliSem());
}
