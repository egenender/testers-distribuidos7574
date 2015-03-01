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

AtendedorDispositivos::AtendedorDispositivos( const Configuracion& config ) { 
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    //Cola requerimientos
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS) );
    m_ColaRequerimientos = msgget(key, 0666);
    if(m_ColaRequerimientos == -1) {
        std::string err = std::string("Error al obtener la cola para enviar requerimientos. Errno: ") 
                          + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola testers
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_TESTERS) );
    m_ColaTests = msgget(key, 0666);
    if(m_ColaTests == -1) {
        std::string err = std::string("Error al obtener la cola para enviar testeos a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola testers especiales
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES) );
    m_ColaTestsEspeciales = msgget(key, 0666);
    if(m_ColaTestsEspeciales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    //Cola dispositivo-config
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_CONFIG) );
    m_ColaDispositivoConfig = msgget(key, 0666);
    if(m_ColaDispositivoConfig == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo, int tipoDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    msg.idTester = 0;
    msg.value = 0;
    msg.tipoDispositivo = tipoDispositivo;
    
    int ret = msgsnd(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar requerimiento al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    m_IdTester = msg.idTester;
    return msg.value;
}

void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    resultado_test_t resultado_test;
    
    resultado_test.mtype = m_IdTester;
    resultado_test.idDispositivo = idDispositivo;
    resultado_test.result = resultado;
            
    int ret = msgsnd(m_ColaTests, &resultado_test, sizeof(resultado_test_t) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirProgramaEspecial(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(m_ColaTestsEspeciales, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_BASE_TAREA_ESPECIAL + idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa especial del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    m_IdTester = msg.idTester;
    m_PosicionDispositivo = msg.posicionDispositivo;
    return msg.value;
}


// El resultado especial se envia solo al Equipo Especial, por lo que el mtype influye poco
void AtendedorDispositivos::enviarResultadoEspecial(int idDispositivo, int resultado) {

    TMessageAtendedor resultadoTestEspecial;
    
    resultadoTestEspecial.mtype = MTYPE_RESULTADO_ESPECIAL;
    resultadoTestEspecial.idDispositivo = idDispositivo;
    resultadoTestEspecial.idTester = m_IdTester;
    resultadoTestEspecial.value = resultado;
    resultadoTestEspecial.posicionDispositivo = m_PosicionDispositivo;
    resultadoTestEspecial.tipoDispositivo = -1;    

    int ret = msgsnd(m_ColaTestsEspeciales, &resultadoTestEspecial, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(m_ColaRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;

}

TMessageDispConfig AtendedorDispositivos::recibirPedidoCambioVariable( int idDispositivo ){
    TMessageDispConfig msg;
    int ret = msgrcv(m_ColaDispositivoConfig, &msg, sizeof(TMessageDispConfig) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir pedido de cambio de variable. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

void AtendedorDispositivos::notificarCambioDeVariableFinalizado( int idDispositivo, bool ultimoCambio ){
    TMessageAtendedor resultadoTestEspecial;
    
    resultadoTestEspecial.mtype = MTYPE_CAMBIO_VAR;
    resultadoTestEspecial.idDispositivo = idDispositivo;
    resultadoTestEspecial.idTester = m_IdTester;
    resultadoTestEspecial.value = ( ultimoCambio? FIN_TEST_CONFIG : 0 );
    resultadoTestEspecial.posicionDispositivo = m_PosicionDispositivo;
    resultadoTestEspecial.tipoDispositivo = -1;
            
    int ret = msgsnd(m_ColaTestsEspeciales, &resultadoTestEspecial, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
