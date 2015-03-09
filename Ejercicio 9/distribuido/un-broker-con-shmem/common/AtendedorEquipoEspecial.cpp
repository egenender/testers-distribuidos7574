#include "AtendedorEquipoEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorEquipoEspecial::AtendedorEquipoEspecial( const Configuracion& config ) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL) );
    m_ColaRecepciones = msgget(key, 0666);
    if( m_ColaRecepciones == -1 ) {
        std::string err = std::string("Error al obtener la cola de recepciones del equipo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_ENVIO_EQUIPO_ESPECIAL) );
    m_ColaEnvios = msgget(key, 0666);
    if( m_ColaEnvios == -1) {
        std::string err = std::string("Error al obtener la cola de envios del equipo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

    sprintf( paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL) );
    sprintf( paramId, "%d", Constantes::ID_EQUIPO_ESPECIAL );
    sprintf( paramSize, "%d", (int) sizeof(TMessageAtendedor) );

    m_PidReceptor = fork();
    if (m_PidReceptor == 0) {
        execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_EMISOR).c_str(),
                paramId, paramIdCola, paramSize, 
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
        exit(1);
    }

    sprintf(paramIdCola, "%d", config.ObtenerParametroEntero(MSGQUEUE_ENVIO_EQUIPO_ESPECIAL) );

    if (fork() == 0) {
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPTOR).c_str(),
                paramId, paramIdCola, paramSize, 
                (char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
        exit(1);
    }
}

AtendedorEquipoEspecial::~AtendedorEquipoEspecial() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", m_PidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}

TMessageAtendedor AtendedorEquipoEspecial::recibirResultadoEspecial() {
    TMessageAtendedor resultado;
    int ret = msgrcv( m_ColaRecepciones, &resultado, sizeof(TMessageAtendedor) - sizeof(long), 0, 0 );
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado especial de algun dispositivo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return resultado;
}

void AtendedorEquipoEspecial::enviarFinTestEspecialADispositivo(int idDispositivo) {
    TMessageAtendedor msg;
    msg.mtype = Constantes::ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_FIN_TEST_ESPECIAL;
    msg.idDispositivo = idDispositivo;
    msg.value = Constantes::FIN_TEST_ESPECIAL;
    int ret = msgsnd(m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar fin de testeo especial al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorEquipoEspecial::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = Constantes::ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_ORDEN;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    int ret = msgsnd(m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar orden de apagado o reinicio al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
