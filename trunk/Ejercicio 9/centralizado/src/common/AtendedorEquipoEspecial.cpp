/* 
 * File:   AtendedorEquipoEspecial.cpp
 * Author: ferno
 * 
 * Created on November 17, 2014, 12:59 AM
 */

#include "AtendedorEquipoEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;
using std::string;

AtendedorEquipoEspecial::AtendedorEquipoEspecial( const Configuracion& config ) {
    const string ipcFileName = config.ObtenerParametroString( ARCHIVO_IPCS );
    key_t key = ftok( ipcFileName.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES) );
    m_ColaDispTesterEsp = msgget(key, 0666);
    if(m_ColaDispTesterEsp == -1) {
        std::string err = std::string("Error al obtener la cola para enviar fin de tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS) );
    m_ColaOrdenDispositivos = msgget(key, 0666);
    if(m_ColaOrdenDispositivos == -1) {
        std::string err = std::string("Error al obtener la cola para enviar orden de apagado o reinicio a dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
}

AtendedorEquipoEspecial::~AtendedorEquipoEspecial() {
}

TMessageAtendedor AtendedorEquipoEspecial::recibirResultadoEspecial() {
    TMessageAtendedor resultado;
    //El -MTYPE_CAMBIO_VAR es para recibir solo MTYPE_CAMBIO_VAR (4001) y MTYPE_RESULTADO_ESPECIAL (2001)
    int ret = msgrcv(m_ColaDispTesterEsp, &resultado, sizeof(TMessageAtendedor) - sizeof(long), -MTYPE_CAMBIO_VAR, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado especial de algun dispositivo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return resultado;
}

void AtendedorEquipoEspecial::enviarFinTestEspecialADispositivo(int idDispositivo) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.value = FIN_TEST_ESPECIAL;
    int ret = msgsnd(m_ColaDispTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
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
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = -1;
    msg.value = orden;
    int ret = msgsnd(m_ColaOrdenDispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar orden de apagado o reinicio al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
