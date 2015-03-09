/* 
 * File:   AtendedorTestersEspeciales.cpp
 * Author: knoppix
 * 
 * Created on February 5, 2015, 1:02 AM
 */

#include "AtendedorTestersEspeciales.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorTestersEspeciales::AtendedorTestersEspeciales( int idTester, const Configuracion& config ) :
        m_IdTester( idTester ) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTER_ESPECIAL) );
    m_ColaEnvios = msgget( key, 0666 );
    if( m_ColaEnvios == -1 ) {
        std::string err = std::string("Error al obtener la cola de envios del tester especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_REQ_TESTER_ESPECIAL) );
    m_ColaRecepcionesReq = msgget(key, 0666);
    if( m_ColaRecepcionesReq == -1 ) {
        std::string err = std::string("Error al obtener la cola de recepcion del tester especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramId[10];
    sprintf( paramId, "%d", m_IdTester );
    char paramCola[10];
    sprintf( paramCola, "%d",
             config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL) );
    char paramSize[10];
    sprintf(paramSize, "%d", (int) sizeof(TMessageAtendedor));
    m_PidReceptor = fork();
    if( m_PidReceptor == 0 ){
        execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_EMISOR).c_str(),
                paramId, paramCola, paramSize, (char*)0);
        exit(1);
    }

    sprintf(paramCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTER_ESPECIAL) );

    if (fork() == 0){
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPTOR).c_str(),
                paramId, paramCola, paramSize, 
                (char*)0);
        exit(1);
    }

    registrarTester();
}

AtendedorTestersEspeciales::~AtendedorTestersEspeciales() {
    char pidToKill[10];
    sprintf( pidToKill, "%d", m_PidReceptor );
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}

TMessageAtendedor AtendedorTestersEspeciales::recibirRequerimientoEspecial(int idEsp) {

    TMessageAtendedor msg;
    int ret = msgrcv( m_ColaRecepcionesReq, &msg, sizeof(TMessageAtendedor) - sizeof(long), idEsp, 0 );
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

void AtendedorTestersEspeciales::enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_TAREA_ESPECIAL;
    msg.tester = m_IdTester;
    msg.idDispositivo = idDispositivo;
    msg.posicionDispositivo = posicionDispositivo;
    msg.value = tarea;

    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0 );
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar tarea especial " << tarea << " al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTestersEspeciales::enviarDisponibilidad() {

    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_AVISAR_DISPONIBILIDAD;
    msg.tester = m_IdTester;
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0 );
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar aviso de disponibilidad  del tester especial " << m_IdTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTestersEspeciales::registrarTester() {

    // El primer mensaje que se envia es de registro TODO: Ver devolucion!
    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_REGISTRAR_TESTER;
    msg.esTesterEspecial = true;
    msg.tester = m_IdTester;
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0 );
    if(ret == -1) {
        std::string error = std::string("Error al enviar mensaje de registro de tester especial. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}
