#include "AtendedorTesters.h"
#include "Configuracion.h"
#include <cstdlib>
#include <stdexcept>

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorTesters::AtendedorTesters( const Configuracion& config ):
        m_CantTestersEspeciales( config.ObtenerParametroEntero(CANT_TESTERS_ESPECIALES) ),
        m_IdPrimerTesterEspecial( config.ObtenerParametroEntero(ID_TESTER_ESPECIAL_START) ),
        m_SemColaEspeciales( config.ObtenerParametroString(ARCHIVO_IPCS),
                             config.ObtenerParametroEntero(SEM_COLA_ESPECIALES) ){
    //Cola requerimientos
    key_t key;
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS) );
    m_ColaRequerimientos = msgget(key, 0666);
    if(m_ColaRequerimientos == -1) {
        std::string err = std::string("Error al obtener la cola de requerimientos del atendedor de testers. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola testers
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_TESTERS) );
    m_ColaRecibosTests = msgget(key, 0666);
    if(m_ColaRecibosTests == -1) {
        std::string err = std::string("Error al obtener la cola de lectura de resultados del atendedor de testers. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola testers especiales
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_TESTERS_ESPECIALES) );
    m_ColaTestersEspeciales = msgget(key, 0666);
    if(m_ColaTestersEspeciales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar a los testers especiales. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola dispositivos-testers especiales
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES) );
    m_ColaTareasEspeciales = msgget(key, 0666);
    if(m_ColaTareasEspeciales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola testers-testers configuracion
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_TESTERS_CONFIG) );
    m_ColaTestersConfig = msgget(key, 0666);
    if(m_ColaTestersConfig == -1) {
        std::string err = std::string("Error al obtener la cola para enviar requerimientos a los testers de config. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola dispositivo-config
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_CONFIG) );
    m_ColaDispositivosConfig = msgget(key, 0666);
    if(m_ColaDispositivosConfig == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    m_SemColaEspeciales.getSem();
}

AtendedorTesters::~AtendedorTesters() {
}

TMessageAtendedor AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = tester;
    msg.value = idPrograma;
    
    int ret = msgsnd(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

resultado_test_t AtendedorTesters::recibirResultado(int idTester) {
    resultado_test_t rsp;
    int ret = msgrcv(m_ColaRecibosTests, &rsp, sizeof(resultado_test_t) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
        
    return rsp;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idTester = -1;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = MTYPE_BASE_TAREA_ESPECIAL + idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = idTester;
    msg.posicionDispositivo = posicionDispositivo;
    msg.value = tarea;

    int ret = msgsnd(m_ColaTareasEspeciales, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar tarea especial " << tarea << " al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(std::vector<bool> cuales, int idDispositivo, int posicionDispositivo){
    m_SemColaEspeciales.p();
    for (int i = 0; i < m_CantTestersEspeciales; i++){
            if (!cuales[i]) continue;
            TMessageAssignTE msg;
            msg.mtype = i + m_IdPrimerTesterEspecial;
            msg.idDispositivo = idDispositivo;
            msg.posicionDispositivo = posicionDispositivo;
            std::stringstream ss;
            ss << "Se envia requerimiento especial a tester especial " << msg.mtype;
            Logger::debug(ss.str(), __FILE__);
            ss.str("");
            int ret = msgsnd(m_ColaTestersEspeciales, &msg, sizeof(TMessageAssignTE) - sizeof(long), 0);
            if(ret == -1) {
                std::string error = std::string("Error al asignar dispositivos a testers especiales. Error: ") + std::string(strerror(errno));
                Logger::error(error.c_str(), __FILE__);
                exit(0);
            }
    }
    m_SemColaEspeciales.v();
}

TMessageAssignTE AtendedorTesters::recibirRequerimientoEspecial(int idEsp) {

    TMessageAssignTE msg;
    int ret = msgrcv(m_ColaTestersEspeciales, &msg, sizeof(TMessageAssignTE) - sizeof(long), idEsp, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

void AtendedorTesters::enviarReqTestConfig( int idDispositivo, int idTester, int tipoDispositivo ){
    TMessageTesterConfig msg;

    msg.mtype = idTester;
    msg.idDispositivo = idDispositivo;
    msg.tipoDispositivo = tipoDispositivo;
    int ret = msgsnd(m_ColaTestersConfig, &msg, sizeof(TMessageTesterConfig) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar test config al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

TMessageTesterConfig AtendedorTesters::recibirReqTestConfig( int idTester ){
    TMessageTesterConfig msg;

    int ret = msgrcv(m_ColaTestersConfig, &msg, sizeof(TMessageTesterConfig) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento de test config del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

void AtendedorTesters::enviarCambioVariable( int idDispositivo, int idTesterConfig, int idVariable, int nuevoValor, bool ultimo ){
    TMessageDispConfig msg;
    msg.mtype = idDispositivo;
    msg.idVariable = idVariable;
    msg.nuevoValor = nuevoValor;
    msg.idTesterConfig = idTesterConfig;
    msg.ultimo = ultimo;
    int ret = msgsnd(m_ColaDispositivosConfig, &msg, sizeof(TMessageDispConfig) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar cambio de variable al dispositivo " << idDispositivo << " desde tester-config. Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
