/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorDispositivos::AtendedorDispositivos(int idDispositivo, const Configuracion& config) :
        m_IdDispositivo(idDispositivo) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_DISP) );
    m_ColaEnvios = msgget(key, 0666);
    if( m_ColaEnvios == -1 ) {
        std::string err = std::string("Error al obtener la cola para enviar mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_DISP) );
    m_ColaRecepciones = msgget(key, 0666);
    if( m_ColaRecepciones == -1 ) {
        std::string err = std::string("Error al obtener la cola para recibir mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

    sprintf(paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_DISP) );
    sprintf(paramId, "%d", m_IdDispositivo);
    sprintf(paramSize, "%d", (int) sizeof(TMessageAtendedor) );

    m_PidReceptor = fork();
    if (m_PidReceptor == 0) {
        execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_EMISOR_DISPOSITIVOS).c_str(),
                paramId, paramIdCola, paramSize, 
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
        exit(1);
    }

    sprintf( paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_ENVIO_DISP) );

    m_PidEmisor = fork();
    if (m_PidEmisor == 0) {
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPTOR_DISPOSITIVOS).c_str(),
                paramId, paramIdCola, paramSize,
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
        exit(1);
    }

}

AtendedorDispositivos::~AtendedorDispositivos() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", m_PidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = m_IdDispositivo;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_REQUERIMIENTO_DISPOSITIVO;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar requerimiento al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv( m_ColaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    m_IdTester = msg.tester;
    return msg.value;
}

void AtendedorDispositivos::enviarResultado( int idDispositivo, int resultado ) {

    TMessageAtendedor msg;
    msg.mtype = m_IdDispositivo;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_RESULTADO_INICIAL;
    msg.tester = m_IdTester;
    msg.idDispositivo = m_IdDispositivo;
    msg.value = resultado;

    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if( ret == -1 ) {
        std::string error = std::string("Error al enviar resultado al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirProgramaEspecial( int idDispositivo ) {

    TMessageAtendedor msg;
    int ret = msgrcv( m_ColaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa especial del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    m_IdTester = msg.tester;
    m_PosicionDispositivo = msg.posicionDispositivo;
    return msg.value;
}

// El resultado especial se envia solo al Equipo Especial, por lo que el mtype influye poco
void AtendedorDispositivos::enviarResultadoEspecial( int idDispositivo, int resultado ) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_RESULTADO_ESPECIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = m_IdTester;
    msg.value = resultado;
    msg.posicionDispositivo = m_PosicionDispositivo;
            
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv( m_ColaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;
}
