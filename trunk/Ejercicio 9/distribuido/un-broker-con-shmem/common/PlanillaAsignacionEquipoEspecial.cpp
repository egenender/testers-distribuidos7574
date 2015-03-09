#include "PlanillaAsignacionEquipoEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

PlanillaAsignacionEquipoEspecial::PlanillaAsignacionEquipoEspecial( const Configuracion& config ) {

    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    // Obtengo la msgqueue de envio y recepcion de shmem
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    m_ShmemMsgqueueEmisor = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueEmisor == -1 ) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }

    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
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

    sprintf( paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION) );
    sprintf( paramId, "%d", Constantes::ID_EQUIPO_ESPECIAL );
    sprintf( paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion) );

    m_PidReceptor = fork();
    if (m_PidReceptor == 0) {
        execlp( "./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
        exit(1);
    }
}

PlanillaAsignacionEquipoEspecial::~PlanillaAsignacionEquipoEspecial() {
}

void PlanillaAsignacionEquipoEspecial::registrarTareaEspecialFinalizada(int idDispositivo) {
    obtenerMemoriaCompartida();
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas += 1;
    devolverMemoriaCompartida();
}

bool PlanillaAsignacionEquipoEspecial::terminoTesteoEspecial(int posDispositivo, int idDispositivo) {
    obtenerMemoriaCompartida();
    int cantTareasPendientes = m_Memoria.cantTareasEspecialesAsignadas[posDispositivo].cantTareasEspecialesTotal - m_Memoria.cantTareasEspecialesAsignadas[posDispositivo].cantTareasEspecialesTerminadas;
    int cantTestersPendientes = m_Memoria.cantTestersEspecialesAsignados[posDispositivo].cantTestersEspecialesTotal - m_Memoria.cantTestersEspecialesAsignados[posDispositivo].cantTestersEspecialesTerminados;
    devolverMemoriaCompartida();
    std::stringstream ss;
    ss << "Para dispositivo " << idDispositivo << " con posicion " << posDispositivo << ". Cant Tareas pendientes: " << cantTareasPendientes << " y cant testers pendientes: " << cantTestersPendientes;
    Logger::warn(ss.str(), __FILE__);
    return ((cantTareasPendientes == 0) && (cantTestersPendientes == 0));
}

void PlanillaAsignacionEquipoEspecial::reiniciarContadoresTesteoEspecial(int idDispositivo) {
    obtenerMemoriaCompartida();
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas = 0;
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal = 0;
    m_Memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados = 0;
    devolverMemoriaCompartida();
}

void PlanillaAsignacionEquipoEspecial::limpiarContadoresFinTesteo(int idDispositivo) {
    obtenerMemoriaCompartida();
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTotal = 0;
    m_Memoria.cantTareasEspecialesAsignadas[idDispositivo].cantTareasEspecialesTerminadas = 0;
    m_Memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTotal = 0;
    m_Memoria.cantTestersEspecialesAsignados[idDispositivo].cantTestersEspecialesTerminados = 0;
    devolverMemoriaCompartida(); 
}

void PlanillaAsignacionEquipoEspecial::obtenerMemoriaCompartida() {
    TRequerimientoSharedMemory req;
    req.mtype = Constantes::Mtypes::MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION;
    req.idSolicitante = Constantes::ID_EQUIPO_ESPECIAL;
    std::stringstream log; log << "Equipo Especial " << Constantes::ID_EQUIPO_ESPECIAL << " pide la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    int okSend = msgsnd( m_ShmemMsgqueueReq, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el equipo especial " << Constantes::ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << Constantes::ID_EQUIPO_ESPECIAL << ". Espero la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
    // Espero por la shmem
    int okRead = msgrcv( m_ShmemMsgqueueReceptor, &m_Memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), Constantes::ID_EQUIPO_ESPECIAL, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el equipo especial " << Constantes::ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << Constantes::ID_EQUIPO_ESPECIAL << ". Me llega la shmem asignacion";
    //Logger::debug(log.str(), __FILE__);
}

void PlanillaAsignacionEquipoEspecial::devolverMemoriaCompartida() {
    std::stringstream log; log << "Equipo Especial " << Constantes::ID_EQUIPO_ESPECIAL << " devuelve la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    m_Memoria.mtype = Constantes::Mtypes::MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION;
    int okSend = msgsnd( m_ShmemMsgqueueEmisor, &m_Memoria, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el equipo especial " << Constantes::ID_EQUIPO_ESPECIAL;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    log << "Equipo Especial " << Constantes::ID_EQUIPO_ESPECIAL << " devuelta la shmem asignacion";
    //Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
}
