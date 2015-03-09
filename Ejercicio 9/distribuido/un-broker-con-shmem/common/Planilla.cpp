/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "Configuracion.h"
#include "logger/Logger.h"

using namespace Constantes::NombresDeParametros;
using std::string;

Planilla::Planilla( int idTester, const Configuracion& config ) :
        m_IdTester(idTester) {

    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    // Obtengo la msgqueue de envio y recepcion de shmem
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL) );
    m_ShmemMsgqueueEmisor = msgget( key, IPC_CREAT | 0660 );
    if( m_ShmemMsgqueueEmisor == -1 ) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL) );
    m_ShmemMsgqueueReceptor = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueReceptor == -1 ) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS) );
    m_ShmemMsgqueueReq = msgget(key, IPC_CREAT | 0660);
    if( m_ShmemMsgqueueReq == -1 ) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }

    // Creo la comunicacion al broker para la memoria compartida distribuida
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

    sprintf( paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL) );
    sprintf( paramId, "%d", m_IdTester + Constantes::Mtypes::INIT_MTYPE_SHMEM_PLANILLA_GENERAL );
    sprintf( paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral) );

    m_PidReceptor = fork();
    if( m_PidReceptor == 0) {
        execlp( "./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_ENVIO_SHM_PLANILLA_GENERAL).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
        exit(1);
    }

    sprintf(paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL) );
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes

    m_PidEmisor = fork();
    if( m_PidEmisor == 0 ) {
        execlp( "./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
        exit(1);
    }
}

Planilla::~Planilla() {
}

int Planilla::hayLugar() {

    // Si hay lugar, registro el dispositivo
    int result = SIN_LUGAR;
    // Obtengo la memoria compartida
    obtenerSharedMemory();
    if( m_Memoria.cantDispositivosSiendoTesteados < Constantes::MAX_DISPOSITIVOS_EN_SISTEMA ) {
        m_Memoria.cantDispositivosSiendoTesteados += 1;
        int i = 0;
        for (i = 0; (i < Constantes::MAX_DISPOSITIVOS_EN_SISTEMA) && (m_Memoria.idsPrivadosDispositivos[i]); i++)
            ;
        m_Memoria.idsPrivadosDispositivos[i] = true;
        result = i;
    }
    devolverSharedMemory();
    return result;
}
void Planilla::eliminarDispositivo(int posicionDispositivo) {
    obtenerSharedMemory();
    m_Memoria.cantDispositivosSiendoTesteados -= 1;
    m_Memoria.idsPrivadosDispositivos[posicionDispositivo] = false;
    devolverSharedMemory();
}

void Planilla::obtenerSharedMemory() {
    TRequerimientoSharedMemory req;
    req.mtype = Constantes::Mtypes::MTYPE_REQ_SHMEM_PLANILLA_GENERAL;
    req.idDevolucion = m_IdTester + Constantes::Mtypes::INIT_MTYPE_SHMEM_PLANILLA_GENERAL;
    req.idSolicitante = m_IdTester;
    int okSend = msgsnd( m_ShmemMsgqueueReq, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el tester " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    // Espero por la shmem
    int okRead = msgrcv( m_ShmemMsgqueueReceptor, &m_Memoria, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long),
                         m_IdTester + Constantes::Mtypes::INIT_MTYPE_SHMEM_PLANILLA_GENERAL, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el tester " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    std::stringstream ss; ss << "Me llega la shmem de planilla general al tester/equipo " << m_Memoria.mtype;
    //Logger::debug(ss.str(), __FILE__);
}

void Planilla::devolverSharedMemory() {
    std::stringstream ss; ss << "Tester/equipo " << m_IdTester << " envía planilla general de vuelta";
    //Logger::debug(ss.str(), __FILE__);
    m_Memoria.mtype = Constantes::Mtypes::MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL;
    int okSend = msgsnd( m_ShmemMsgqueueEmisor, &m_Memoria, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el tester " << m_IdTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
}
