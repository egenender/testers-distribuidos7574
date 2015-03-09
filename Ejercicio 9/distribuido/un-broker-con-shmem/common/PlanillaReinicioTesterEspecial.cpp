/* 
 * File:   PlanillaReinicioTesterEspecial.cpp
 * Author: knoppix
 * 
 * Created on November 17, 2014, 12:19 AM
 */

#include "PlanillaReinicioTesterEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;

PlanillaReinicioTesterEspecial::PlanillaReinicioTesterEspecial( const Configuracion& config ) {
    key_t key = ftok( config.ObtenerParametroString( ARCHIVO_IPCS ).c_str(),
                      config.ObtenerParametroEntero( MSGQUEUE_REINICIO_TESTER_ESPECIAL ) );
    m_MsgqReinicioId = msgget( key, 0666 );
    if( m_MsgqReinicioId == -1) {
        std::string err = std::string("Error al obtener la cola de reinicio de tests. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
}

PlanillaReinicioTesterEspecial::~PlanillaReinicioTesterEspecial() {
}

bool PlanillaReinicioTesterEspecial::hayQueReiniciar(int idTester) {

    std::stringstream log; log << "Tester especial " << idTester << " espera por msg de la msgqueue para ver si hay que reiniciar";
    Logger::debug(log.str(), __FILE__); log.str(""); log.clear();
    TMessageAtendedor msg;
    int ret = msgrcv( m_MsgqReinicioId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0 );
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al recibir reinicio de testeo para tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    log << "Tester especial: " << idTester << ". Me llego respuesta de reinicio o no";
    Logger::debug(log.str());
    return msg.hayQueReiniciar;
}
