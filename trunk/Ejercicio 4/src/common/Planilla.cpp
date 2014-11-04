/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "common/common.h"
#include "logger/Logger.h"
#include <sys/shm.h>
#include <cerrno>
#include <cstring>

Planilla::Planilla() : m_SemShMem( Constantes::ARCHIVO_IPCS, Constantes::SEM_PLANILLA) {

    m_ShMemKey = ftok(Constantes::ARCHIVO_IPCS.c_str(), Constantes::SHMEM_PLANILLA);
    if(m_ShMemKey == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_ShMemId = shmget(m_ShMemKey, sizeof(int), IPC_CREAT);
    if(m_ShMemId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }

    void* pShm = shmat (m_ShMemId , NULL ,0);
    if ( pShm != (void*) -1 ) {
        m_pCantDispositivosSiendoTesteados = static_cast<int*> (pShm);
        Logger::debug("Memoria compartida de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if (!m_SemShMem.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
}

Planilla::~Planilla() {}

bool Planilla::hayLugar() {

    // Si hay lugar, registro el dispositivo
    bool result = true;
    m_SemShMem.p();
    if(*m_pCantDispositivosSiendoTesteados >= Constantes::MAX_DISPOSITIVOS_EN_SISTEMA) {
        result = false;
    } else {
        *m_pCantDispositivosSiendoTesteados += 1;
    }
    m_SemShMem.v();
    return result;
}

void Planilla::eliminarDispositivo() {

    m_SemShMem.p();
    *m_pCantDispositivosSiendoTesteados -= 1;
    m_SemShMem.v();
}

int Planilla::cantProcesosUsandoPlanilla() {
    shmid_ds estado;
    if (shmctl(m_ShMemId, IPC_STAT, &estado) == -1) {
        std::string err = std::string("Error en shmctl() al verificar la cantidad de procesos adosados a la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
    }
    return estado.shm_nattch;
}

bool Planilla::destruirMemoria() {

    return (shmctl(m_ShMemId, IPC_RMID, NULL) != -1);
}

bool Planilla::destruirSemaforo() {

    return (m_SemShMem.eliSem());
}

