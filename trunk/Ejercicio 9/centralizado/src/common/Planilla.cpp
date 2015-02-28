/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "Configuracion.h"
#include "logger/Logger.h"
#include "common.h"

using namespace Constantes::NombresDeParametros;
using std::string;

Planilla::Planilla( const Configuracion& config ) :
        m_MaxDispositivosEnSistema( config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA) ),
        m_SemShMem( config.ObtenerParametroString(ARCHIVO_IPCS), config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL) ) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    //Mutex
    if ( !m_SemShMem.getSem() ) {
        std::string err = std::string("Error al obtener el semaforo de la planilla general. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    //Shm planilla
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL) );
    if( key == -1 ) {
        std::string err("Error al conseguir la key de la shmem de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_ShMemId = shmget( key, sizeof(int), 0660 );
    if( m_ShMemId == -1 ) {
        std::string err("Error al conseguir la memoria compartida de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    void* tmpPtr = shmat( m_ShMemId, NULL, 0 );
    if ( tmpPtr != (void*) -1 ) {
        m_pCantDispositivosSiendoTesteados = static_cast<int*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla general. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    //Shm posiciones
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL_POSICIONES) );
    if( key == -1 ) {
        std::string err("Error al conseguir la key de la shmem de posiciones de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_ShMemPosicionesId = shmget( key, sizeof(bool) * config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA), 0660 );
    if( m_ShMemPosicionesId == -1 ) {
        std::string err("Error al conseguir la memoria compartida de posiciones de la planilla general. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }    
    tmpPtr = shmat( m_ShMemPosicionesId, NULL, 0 );
    if ( tmpPtr != (void*) -1 ) {
        m_pIdsPrivadosDispositivos = static_cast<bool*> (tmpPtr);
        Logger::debug("Memoria compartida de posiciones de la planilla creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla general en shmem de posiciones. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }    
}

Planilla::~Planilla() {
}

int Planilla::hayLugar() {
    // Si hay lugar, registro el dispositivo
    int result = SIN_LUGAR;
    m_SemShMem.p();
    if( *m_pCantDispositivosSiendoTesteados < m_MaxDispositivosEnSistema ) {
        *m_pCantDispositivosSiendoTesteados += 1;
        int i = 0;
        for (i = 0; (i < m_MaxDispositivosEnSistema) && (m_pIdsPrivadosDispositivos[i]); i++);
            m_pIdsPrivadosDispositivos[i] = true;
        result = i;
    }
    m_SemShMem.v();
    return result;
}

void Planilla::eliminarDispositivo(int posicionDispositivo) {
    m_SemShMem.p();
    *m_pCantDispositivosSiendoTesteados -= 1;
    m_pIdsPrivadosDispositivos[posicionDispositivo] = false;
    m_SemShMem.v();
}

int Planilla::cantProcesosUsandoPlanilla() {
    shmid_ds estado;
    if( shmctl(m_ShMemId, IPC_STAT, &estado) == -1 ){
        std::string err = std::string("Error en shmctl() al verificar la cantidad de procesos adosados a la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
    }
    return estado.shm_nattch;
}
