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
    this->colaDispTesterEsp = msgget(key, 0666);
    if(this->colaDispTesterEsp == -1) {
        std::string err = std::string("Error al obtener la cola para enviar fin de tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_REINICIO_TESTEO) );
    this->colaReinicioTestEsp = msgget(key, 0666);
    if(this->colaReinicioTestEsp == -1) {
        std::string err = std::string("Error al obtener la cola para avisar reinicio de testeo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok( ipcFileName.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_DISPOSITIVOS) );
    this->colaOrdenDispositivos = msgget(key, 0666);
    if(this->colaOrdenDispositivos == -1) {
        std::string err = std::string("Error al obtener la cola para enviar orden de apagado o reinicio a dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
}

AtendedorEquipoEspecial::~AtendedorEquipoEspecial() {
}

TResultadoEspecial AtendedorEquipoEspecial::recibirResultadoEspecial() {
    TResultadoEspecial resultado;
    int ret = msgrcv(this->colaDispTesterEsp, &resultado, sizeof(TResultadoEspecial) - sizeof(long), MTYPE_RESULTADO_ESPECIAL, 0);
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
    int ret = msgsnd(this->colaDispTesterEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
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
    int ret = msgsnd(this->colaOrdenDispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar orden de apagado o reinicio al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
