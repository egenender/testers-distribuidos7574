/* 
 * File:   PlanillaReinicioEquipoEspecial.cpp
 * Author: knoppix
 * 
 * Created on November 17, 2014, 1:35 PM
 */

#include "PlanillaReinicioEquipoEspecial.h"
#include "Configuracion.h"
#include "common.h"

using namespace Constantes::NombresDeParametros;

PlanillaReinicioEquipoEspecial::PlanillaReinicioEquipoEspecial( const Configuracion& config ) {
    key_t key = ftok( config.ObtenerParametroString(ARCHIVO_IPCS).c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_ENVIO_EQUIPO_ESPECIAL) );
    m_MsgqReinicioId = msgget( key, 0666 );
    if( m_MsgqReinicioId == -1 ) {
        std::string err = std::string("Error al obtener la cola de reinicio de tests de la planilla de reinicio del equipo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
}

PlanillaReinicioEquipoEspecial::~PlanillaReinicioEquipoEspecial() {
}

void PlanillaReinicioEquipoEspecial::avisarReinicio(const std::set<int>& idTesters, bool reiniciar) {

    TMessageAtendedor msg;
    msg.mtype = Constantes::ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = Constantes::Mtypes::MTYPE_HAY_QUE_REINICIAR;
    msg.hayQueReiniciar = reiniciar;
    for (std::set<int>::const_iterator it = idTesters.begin(); it != idTesters.end(); it++) {
        msg.tester = *it;
        std::stringstream ss;
        ss << "Se avisa del reinicio al tester especial " << *it;
        Logger::debug(ss.str(), __FILE__);
        int ret = msgsnd( m_MsgqReinicioId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0 );
        if(ret == -1) {
            std::string error = std::string("Error al enviar reinicio de tests desde el equipo especial. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(0);
        }
    }
}
