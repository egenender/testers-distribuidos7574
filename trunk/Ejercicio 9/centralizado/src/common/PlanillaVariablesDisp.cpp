#include "PlanillaVariablesDisp.h"
#include "Configuracion.h"
#include <cassert>

using namespace Constantes::NombresDeParametros;
using std::string;

PlanillaVariablesDisp::PlanillaVariablesDisp( const Configuracion& config, int idDisp ) : //TODO <REF> Renombrar clase
        m_IdDisp( idDisp ),
        m_MutexPlanilla( config.ObtenerParametroString( ARCHIVO_IPCS ),
                         config.ObtenerParametroEntero( SEM_MUTEX_PLANILLA_VARS_START ) + idDisp ),
        m_SemTestsEspeciales( config.ObtenerParametroString( ARCHIVO_IPCS ),
                             config.ObtenerParametroEntero( SEM_PLANILLA_VARS_TE_START ) + idDisp ),
        m_SemCambioVars( config.ObtenerParametroString( ARCHIVO_IPCS ),
                         config.ObtenerParametroEntero( SEM_PLANILLA_VARS_CV_START ) + idDisp ){
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    std::stringstream nombre;
    nombre << __FILE__ << " " << idDisp;
    //Shm
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(SHM_PLANILLA_VARS_START) + idDisp );
    if( key == -1 ) {
        std::string err( "Error al conseguir la key de la shmem de la planilla de variables. Error: " + std::string(strerror(errno)) );
        Logger::error(err.c_str(), nombre.str().c_str());
        throw err;
    }
    m_ShmemEstadoId = shmget( key, sizeof(TEstadoDispositivo), 0660 );
    if(m_ShmemEstadoId == -1) {
        std::string err( "Error al conseguir la memoria compartida de la planilla de variables. Error: " + std::string(strerror(errno)) );
        Logger::error(err.c_str(), nombre.str().c_str());
        throw err;
    }    
    void* tmpPtr = shmat(m_ShmemEstadoId, NULL ,0);
    if ( tmpPtr != (void*) -1 ) {
        m_pShmEstado = static_cast<TEstadoDispositivo*> (tmpPtr);
        Logger::debug( "Memoria compartida de la planilla de variables creada correctamente" );
    } else {
        std::string err = std::string("Error en shmat() de planilla de variables. Error: ") + std::string(strerror(errno) );
        Logger::error(err, nombre.str().c_str());
        throw err;
    }
    
    //Semaforos
    if (!m_MutexPlanilla.getSem()) {
        std::string err = std::string("Error al obtener el mutex de la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, nombre.str().c_str());
        throw err;
    }
    if (!m_SemTestsEspeciales.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de tests especiales la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, nombre.str().c_str());
        throw err;
    }
    if (!m_SemCambioVars.getSem()) {
        std::string err = std::string("Error al obtener el semaforo de cambio de variables de la planilla de variables. Error: ")
                          + std::string(strerror(errno));
        Logger::error(err, nombre.str().c_str());
        throw err;
    }
}

PlanillaVariablesDisp::~PlanillaVariablesDisp() {
}

void PlanillaVariablesDisp::iniciarTestEspecial(){
    m_MutexPlanilla.p();
    switch( m_pShmEstado->estadoDispConfig ){
        case EPD_ESPERANDO:
            m_pShmEstado->estadoDisp = EPD_ESPERANDO;
            m_pShmEstado->estadoDispConfig = EPD_OCUPADO;
            m_SemCambioVars.v();
            m_MutexPlanilla.v();
            m_SemTestsEspeciales.p();
            break;
        case EPD_OCUPADO:
            m_pShmEstado->estadoDisp = EPD_ESPERANDO;
            m_MutexPlanilla.v();
            m_SemTestsEspeciales.p();
            break;
        case EPD_LIBRE:
            m_pShmEstado->estadoDisp = EPD_OCUPADO;
            m_MutexPlanilla.v();
            break;
    }    
}
   
void PlanillaVariablesDisp::finalizarTestEspecial(){
    m_MutexPlanilla.p();
    m_pShmEstado->estadoDisp = EPD_LIBRE;
    switch( m_pShmEstado->estadoDispConfig ){
        case EPD_ESPERANDO:
            m_pShmEstado->estadoDispConfig = EPD_OCUPADO;
            m_SemCambioVars.v();            
            break;
        case EPD_OCUPADO:
            assert( false );
            break;
        case EPD_LIBRE:
            break;
    }
    m_MutexPlanilla.v();
    m_SemTestsEspeciales.v();
}

void PlanillaVariablesDisp::iniciarCambioDeVariable( int idVar ){
    m_MutexPlanilla.p();
    if( m_pShmEstado->estadoDisp == EPD_OCUPADO ){
        m_pShmEstado->estadoDispConfig = EPD_ESPERANDO;
        m_MutexPlanilla.v();
        m_SemCambioVars.p();        
    }else{
        m_pShmEstado->estadoDispConfig = EPD_OCUPADO;
        m_MutexPlanilla.v();
    }
}
   
void PlanillaVariablesDisp::finalizarCambioDeVariable( int idVar ){
    m_MutexPlanilla.p();
    m_pShmEstado->estadoDispConfig = EPD_LIBRE;
    switch( m_pShmEstado->estadoDisp ){
        case EPD_ESPERANDO:
            m_pShmEstado->estadoDisp = EPD_OCUPADO;
            m_SemTestsEspeciales.v();
            break;
        case EPD_OCUPADO:
            assert( false );            
            break;
        case EPD_LIBRE:
            break;
    }
    m_MutexPlanilla.v();
    m_SemCambioVars.v();    
}
