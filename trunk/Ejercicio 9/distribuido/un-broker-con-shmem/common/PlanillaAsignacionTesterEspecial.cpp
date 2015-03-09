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

PlanillaAsignacionTesterEspecial::PlanillaAsignacionTesterEspecial( int idTester, const Configuracion& config ) :
        m_IdTester(idTester) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    // Obtengo la msgqueue de envio y recepcion de shmem
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    m_ShmemMsgqueueEmisor = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueEmisor == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    int idColaRecepcionTestersShmemPlanillaAsignacion = config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    key = ftok( archivoIpcs.c_str(), idColaRecepcionTestersShmemPlanillaAsignacion );
	m_ShmemMsgqueueReceptor = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueReceptor == -1 ) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }

    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS) );
    m_ShmemMsgqueueReq = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueReq == -1 ) {
        Logger::error("Error al construir la msgqueue para requerimientos de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    // Creo la comunicacion al broker para la memoria compartida distribuida
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

    sprintf( paramIdCola, "%d", idColaRecepcionTestersShmemPlanillaAsignacion );
    sprintf( paramId, "%d", m_IdTester );
    sprintf( paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion) );

    m_PidReceptor = fork();
    if ( m_PidReceptor == 0 ) {
        execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString( UBICACION_SERVER ).c_str(),
                config.ObtenerParametroString( PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION ).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
        exit(1);
    }

    // El emisor es unico para todos los que usan esta planilla. Por eso, se crea en el iniciador
}

void PlanillaAsignacionTesterEspecial::asignarCantTareasEspeciales( int idDispositivo, int cantTareas ){
    obtenerMemoriaCompartida();
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal += cantTareas;
    devolverMemoriaCompartida();
}
void PlanillaAsignacionTesterEspecial::avisarFinEnvioTareas( int idDispositivo ){
    obtenerMemoriaCompartida();
    m_Memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados += 1;
    devolverMemoriaCompartida();
}

PlanillaAsignacionTesterEspecial::~PlanillaAsignacionTesterEspecial() {
}

void PlanillaAsignacionTesterEspecial::obtenerMemoriaCompartida() {
    TRequerimientoSharedMemory req;
    req.mtype = MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION;
    req.idSolicitante = m_IdTester;
    std::stringstream log; log << "Tester Especial " << m_IdTester << " pide la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    int okSend = msgsnd( m_ShmemMsgqueueReq, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el tester especial " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Tester Especial " << m_IdTester << ". Espero la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
    // Espero por la shmem
    int okRead = msgrcv( m_ShmemMsgqueueReceptor, &m_Memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), m_IdTester, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el tester especial " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Tester Especial " << m_IdTester << ". Me llega la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
}

void PlanillaAsignacionTesterEspecial::devolverMemoriaCompartida() {
    std::stringstream log;
    log << "Tester Especial " << m_IdTester << " devuelve la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    m_Memoria.mtype = MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION;
    int okSend = msgsnd( m_ShmemMsgqueueEmisor, &m_Memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el tester especial " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Tester Especial " << m_IdTester << " devuelta la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
}
