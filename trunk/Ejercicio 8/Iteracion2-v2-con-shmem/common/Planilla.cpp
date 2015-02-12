/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "logger/Logger.h"

Planilla::Planilla() : semShMem(SEM_PLANILLA_GENERAL) {
    
    this->shMemKey = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    if(this->shMemKey == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shMemId = shmget(this->shMemKey, sizeof(int), IPC_CREAT | 0660);
    if(this->shMemId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    void* tmpPtr = shmat (this->shMemId , NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        this->cantDispositivosSiendoTesteados = static_cast<int*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla general. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    
    this->shMemPosicionesKey = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_POSICIONES);
    if(this->shMemKey == -1) {
        std::string err("Error al conseguir la key de la shmem de posiciones de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shMemPosicionesId = shmget(this->shMemPosicionesKey, sizeof(bool) * MAX_DISPOSITIVOS_EN_SISTEMA, IPC_CREAT | 0660);
    if(this->shMemId == -1) {
        std::string err("Error al conseguir la memoria compartida de posiciones de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    tmpPtr = shmat (this->shMemPosicionesId , NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        this->idsPrivadosDispositivos = static_cast<bool*> (tmpPtr);
        Logger::debug("Memoria compartida de posiciones de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla general en shmem de posiciones. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if (!this->semShMem.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de la planilla general. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

}

Planilla::Planilla(const Planilla& orig) : semShMem(SEM_PLANILLA_GENERAL) {
}

void Planilla::initPlanilla() {
    this->semShMem.p();
    *(this->cantDispositivosSiendoTesteados) = 0;
    for (int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++)
        this->idsPrivadosDispositivos[i] = false;
    this->semShMem.v();
}

Planilla::~Planilla() {
}

int Planilla::hayLugar() {

    // Si hay lugar, registro el dispositivo
    int result = SIN_LUGAR;
    this->semShMem.p();
    if(*this->cantDispositivosSiendoTesteados >= MAX_DISPOSITIVOS_EN_SISTEMA) {
    } else {
        *this->cantDispositivosSiendoTesteados += 1;
        int i = 0;
        for (i = 0; (i < MAX_DISPOSITIVOS_EN_SISTEMA) && (this->idsPrivadosDispositivos[i]); i++);
        this->idsPrivadosDispositivos[i] = true;
        result = i;
    }
    this->semShMem.v();
    return result;
}
void Planilla::eliminarDispositivo(int posicionDispositivo) {

    this->semShMem.p();
    *this->cantDispositivosSiendoTesteados -= 1;
    this->idsPrivadosDispositivos[posicionDispositivo] = false;
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

    return ((shmctl(this->shMemId, IPC_RMID, NULL) != -1) && (shmctl(this->shMemPosicionesId, IPC_RMID, NULL) != -1));
}

bool Planilla::destruirSemaforo() {

    return (this->semShMem.eliSem());
}
