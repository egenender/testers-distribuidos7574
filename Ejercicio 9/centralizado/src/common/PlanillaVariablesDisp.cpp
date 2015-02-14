
#include "PlanillaVariablesDisp.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

PlanillaVariablesDisp::PlanillaVariablesDisp( const Configuracion& config, int idDisp ) : //TODO <REF> Renombrar
        m_IdDisp( idDisp ),
        m_MutexPlanilla( config.ObtenerParametroString( ARCHIVO_IPCS ),
                         config.ObtenerParametroEntero( SEM_MUTEX_PLANILLA_VARS_START ) + idDisp ),
        m_SemTestEspeciales( config.ObtenerParametroString( ARCHIVO_IPCS ),
                             config.ObtenerParametroEntero( SEM_PLANILLA_VARS_TE_START + idDisp  ) ),
        m_SemCambioVars( config.ObtenerParametroString( ARCHIVO_IPCS ),
                         config.ObtenerParametroEntero( SEM_PLANILLA_VARS_CV_START ) + idDisp ){
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    //Shm
    m_KeyShmemEstado = ftok( ipcFileName.c_str(),
                             config.ObtenerParametroEntero(SHM_PLANILLA_VARS_START) + idDisp );    
    if(m_KeyShmemEstado == -1) {
        std::string err("Error al conseguir la key de la shmem de la planilla de variables. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_KeyShmemEstadoId = shmget(m_KeyShmemEstado, sizeof(TEstadoDispositivo), IPC_CREAT | 0660);
    if(m_KeyShmemEstadoId == -1) {
        std::string err("Error al conseguir la memoria compartida de la planilla de variables. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }    
    void* tmpPtr = shmat(m_KeyShmemEstadoId, NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        m_pShmEstado = static_cast<TContadorTesterEspecial*> (tmpPtr);
        Logger::debug("Memoria compartida de la planilla de variables creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de planilla de variables. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    
    //Semaforos
    if (!m_MutexPlanilla.getSem()) {
        std::string err = std::string("Error al obtener el mutex de la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    if (!this->m_SemTestEspeciales.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de tests especiales la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    if (!this->m_SemCambioVars.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de cambio de variables de la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
}

PlanillaVariablesDisp::~PlanillaVariablesDisp() {
}

void PlanillaVariablesDisp::initPlanilla() {    
}

void PlanillaVariablesDisp::iniciarTestEspecial(){
    m_MutexPlanilla.p();
    switch( m_pShmEstado->EstadoDispConfig ){
        case EDP_ESPERANDO:
            m_pShmEstado->EstadoDisp = EDP_ESPERANDO;
            m_pShmEstado->EstadoDispConfig = EDP_OCUPADO;
            m_SemCambioVars.v();
            m_MutexPlanilla.v();
            m_SemTestsEspeciales.p();
            break;
        case EDP_OCUPADO:
            m_pShmEstado->EstadoDisp = EDP_ESPERANDO;
            m_MutexPlanilla.v();
            m_SemTestsEspeciales.p();
            break;
        case EDP_LIBRE:
            m_pShmEstado->EstadoDisp = EDP_OCUPADO;
            m_MutexPlanilla.v();
            break;
    }    
}
   
void PlanillaVariablesDisp::finalizarTestEspecial(){
    m_MutexPlanilla.p();
    m_pShmEstado->EstadoDisp = EDP_LIBRE;
    switch( m_pShmEstado->EstadoDispConfig ){
        case EDP_ESPERANDO:
            m_pShmEstado->EstadoDispConfig = EDP_OCUPADO;
            m_SemCambioVars.v();            
            break;
        case EDP_OCUPADO:
            assert( false );
            break;        
    }
    m_MutexPlanilla.v();
    m_SemTestEspeciales.v();
}

void PlanillaVariablesDisp::iniciarCambioDeVariable( int idVar ){
    m_MutexPlanilla.p();
    if( m_pShmEstado->EstadoDisp == EDP_OCUPADO ){
        m_pShmEstado->EstadoDispConfig = EDP_ESPERANDO;
        m_MutexPlanilla.v();
        m_SemCambioVars.p();        
    }else{
        m_pShmEstado->EstadoDispConfig = EDP_OCUPADO;
        m_MutexPlanilla.v();
    }
}
   
void PlanillaVariablesDisp::finalizarCambioDeVariable( int idVar ){
    m_MutexPlanilla.p();
    m_pShmEstado->EstadoDispConfig = EDP_LIBRE;
    switch( m_pShmEstado->EstadoDisp ){
        case EDP_ESPERANDO:
            m_pShmEstado->EstadoDisp = EDP_OCUPADO;
            m_SemTestsEspeciales.v();
            break;
        case EDP_OCUPADO:
            assert( false );            
            break;
        case EDP_LIBRE:            
            break;
    }
    m_MutexPlanilla.v();
    m_SemCambioVars.v();    
}
