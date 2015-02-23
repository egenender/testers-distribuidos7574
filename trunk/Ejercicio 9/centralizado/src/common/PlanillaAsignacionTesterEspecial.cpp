/* 
 * File:   PlanillaAsignacionTesterEspecial.cpp
 * Author: knoppix
 * 
 * Created on November 16, 2014, 7:56 PM
 */

#include "PlanillaAsignacionTesterEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

PlanillaAsignacionTesterEspecial::PlanillaAsignacionTesterEspecial( const Configuracion& config ) :
        m_SemShmemCantTesters( config.ObtenerParametroString(ARCHIVO_IPCS),
                               config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS) ),
        m_SemShmemCantTareas( config.ObtenerParametroString(ARCHIVO_IPCS),
                              config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS) ) {
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TESTER_ASIGNADOS) );
    if( key == -1 ) {
        std::string err("Error al conseguir la key de la shmem de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_ShmemCantTestersId = shmget( key, sizeof(TContadorTesterEspecial) * config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA),
                                   0660);
    if( m_ShmemCantTestersId == -1 ) {
        std::string err("Error al conseguir la memoria compartida de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    void* tmpPtr = shmat( m_ShmemCantTestersId, NULL, 0 );
    if ( tmpPtr != (void*) -1 ) {
        m_pCantTestersEspecialesAsignados = static_cast<TContadorTesterEspecial*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla de asignacion de testers creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla de asignacion de testers. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if ( !m_SemShmemCantTesters.getSem() ) {
        std::string err = std::string("Error al obtener el semaforo de la planilla de asignacion de testers. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    
    key = ftok(ipcFileName.c_str(), config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TAREAS_ASIGNADAS) );
    if( key == -1 ) {
        std::string err("Error al conseguir la key de la shmem de la planilla de asignacion de tareas. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_ShmemCantTareasId = shmget( key, sizeof(TContadorTareaEspecial) * config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA), 0660 );
    if( m_ShmemCantTareasId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla de asignacion de tareas. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    tmpPtr = shmat( m_ShmemCantTareasId, NULL, 0 );
    if ( tmpPtr != (void*) -1 ) {
        m_pCantTareasEspecialesAsignadas = static_cast<TContadorTareaEspecial*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla de asignacion de tareas creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla de asignacion de tareas. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    // Por ultimo, luego de creado, obtengo el semaforo correspondiente
    if ( !m_SemShmemCantTareas.getSem() ) {
        std::string err = std::string("Error al obtener el semaforo de la planilla de asignacion de tareas. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
}

void PlanillaAsignacionTesterEspecial::asignarCantTareasEspeciales(int idDispositivo, int cantTareas) {
    m_SemShmemCantTareas.p();
    m_pCantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal += cantTareas;
    m_SemShmemCantTareas.v();
}
void PlanillaAsignacionTesterEspecial::avisarFinEnvioTareas(int idDispositivo) {
    m_SemShmemCantTesters.p();
    m_pCantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados += 1;
    m_SemShmemCantTesters.v();
}

PlanillaAsignacionTesterEspecial::~PlanillaAsignacionTesterEspecial() {
}
