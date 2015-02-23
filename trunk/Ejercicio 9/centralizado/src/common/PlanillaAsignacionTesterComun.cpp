/* 
 * File:   PlanillaAsignacionTesterComun.cpp
 * Author: knoppix
 * 
 * Created on November 16, 2014, 7:56 PM
 */

#include "PlanillaAsignacionTesterComun.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

PlanillaAsignacionTesterComun::PlanillaAsignacionTesterComun( const Configuracion& config ) :
        m_SemShmemCantTesters( config.ObtenerParametroString(ARCHIVO_IPCS),
                               config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS) ){
    const string ipcFileName = config.ObtenerParametroString(ARCHIVO_IPCS);
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TESTER_ASIGNADOS) );
    if( key == -1 ) {
        std::string err("Error al conseguir la key de la shmem de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    const int maxDispositivos = config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA);
    m_ShmemCantTestersId = shmget( key, sizeof(TContadorTesterEspecial) * maxDispositivos, 0660 );
    if( m_ShmemCantTestersId == -1 ) {
        std::string err("Error al conseguir la memoria compartida de la planilla de asignacion de testers. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    
    void* tmpPtr = shmat ( m_ShmemCantTestersId , NULL ,0);
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
}

PlanillaAsignacionTesterComun::~PlanillaAsignacionTesterComun() {
}

void PlanillaAsignacionTesterComun::asignarCantTestersEspeciales(int posicionDispositivo, int cantTestersEspeciales) {
    m_SemShmemCantTesters.p();
    m_pCantTestersEspecialesAsignados[posicionDispositivo].cantTestersEspecialesTotal = cantTestersEspeciales;
    m_SemShmemCantTesters.v();
}
