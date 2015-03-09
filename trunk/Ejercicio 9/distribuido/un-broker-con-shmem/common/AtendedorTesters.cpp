#include "AtendedorTesters.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorTesters::AtendedorTesters( int idTester, const Configuracion& config ):
        m_CantTestersEspeciales( config.ObtenerParametroEntero(CANT_TESTERS_ESPECIALES) ),
        m_IdTester(idTester) {
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    int msgQueueEnvioTesterComun = config.ObtenerParametroEntero(MSGQUEUE_ENVIO_TESTER_COMUN);
    key_t key = ftok( archivoIpcs.c_str(), msgQueueEnvioTesterComun );
    m_ColaEnvios = msgget(key, 0666);
    if( m_ColaEnvios == -1 ) {
        std::string err = std::string("Error al obtener la cola de ENVIOS DEL TESTER. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN) );
    m_ColaRecepcionesGeneral = msgget(key, 0666);
    if( m_ColaRecepcionesGeneral == -1 ) {
        std::string err = std::string("Error al obtener la cola de recepcion de mensajes generales del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    int msgQueueRequerimientosTesterComun = config.ObtenerParametroEntero(MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN);
    key = ftok( archivoIpcs.c_str(), msgQueueRequerimientosTesterComun );
    m_ColaRecepcionesRequerimientos = msgget(key, 0666);
    if( m_ColaRecepcionesRequerimientos == -1 ) {
        std::string err = std::string("Error al obtener la cola de recepcion de requerimientos del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramId[10];
    sprintf(paramId, "%d", m_IdTester);
    char paramCola[10];
    sprintf(paramCola, "%d", msgQueueRequerimientosTesterComun);
    char paramSize[10];
    sprintf(paramSize, "%d", (int) sizeof(TMessageAtendedor));
    m_PidReceptor = fork();
    if( m_PidReceptor == 0 ){
        execlp( "./tcp/tcpclient_receptor", "tcpclient_receptor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_EMISOR).c_str(),
                paramId, paramCola, paramSize, (char*)0);
        exit(1);
    }

    sprintf(paramCola, "%d", msgQueueEnvioTesterComun);

    if (fork() == 0){
        execlp( "./tcp/tcpclient_emisor", "tcpclient_emisor",
                config.ObtenerParametroString(UBICACION_SERVER).c_str(),
                config.ObtenerParametroString(PUERTO_SERVER_RECEPTOR).c_str(),
                paramId, paramCola, paramSize, (char*)0);
        exit(1);
    }
    
    registrarTester();

}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig) {}

AtendedorTesters::~AtendedorTesters() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", m_PidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(m_ColaRecepcionesRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), m_IdTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_PROGRAMA_INICIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = m_IdTester;
    msg.value = idPrograma;
    
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

TMessageAtendedor AtendedorTesters::recibirResultado(int idTester) {
    TMessageAtendedor rsp;
    int ret = msgrcv( m_ColaRecepcionesGeneral, &rsp, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return rsp;
}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_ORDEN;
    msg.tester = m_IdTester;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;

    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(const std::vector<bool>& cuales, int idDispositivo, int posicionDispositivo){
    int j = 0;
    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_REQUERIMIENTO_TESTER_ESPECIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = m_IdTester;
    msg.posicionDispositivo = posicionDispositivo;
    msg.cantTestersEspecialesAsignados = 0;
    for (int i = 0; (i < m_CantTestersEspeciales) && (j < Constantes::MAX_TESTERS_ESPECIALES_PARA_ASIGNAR); i++) {
        if (!cuales[i])
            continue;
        msg.idTestersEspeciales[j] = i + Constantes::ID_TESTER_ESP_START;
        j++;
        msg.cantTestersEspecialesAsignados++;
    }

    std::stringstream ss;
    ss << "Se envian requerimientos especiales a testers especiales: " << msg.idTestersEspeciales[0] << ", " << msg.idTestersEspeciales[1] << ", " << msg.idTestersEspeciales[2] << ", " << msg.idTestersEspeciales[3];
    Logger::debug(ss.str(), __FILE__);
    ss.str("");
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al asignar dispositivo a testers especiales. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

bool AtendedorTesters::destruirComunicacion() {
    return ( msgctl(m_ColaEnvios, IPC_RMID, (struct msqid_ds*)0) != -1
             &&
             msgctl(m_ColaRecepcionesGeneral, IPC_RMID, (struct msqid_ds*)0) != -1
             &&
             msgctl(m_ColaRecepcionesRequerimientos, IPC_RMID, (struct msqid_ds*)0) != -1 );
}

void AtendedorTesters::registrarTester() {

    // El primer mensaje que se envia es de registro TODO: Ver devolucion!
    TMessageAtendedor msg;
    msg.mtype = m_IdTester;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_REGISTRAR_TESTER;
    msg.esTesterEspecial = false;
    msg.tester = m_IdTester;
    int ret = msgsnd( m_ColaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar mensaje de registro de tester comun. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
