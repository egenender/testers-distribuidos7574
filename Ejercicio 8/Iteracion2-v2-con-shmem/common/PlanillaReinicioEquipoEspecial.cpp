/* 
 * File:   PlanillaReinicioEquipoEspecial.cpp
 * Author: ferno
 * 
 * Created on November 17, 2014, 1:35 PM
 */

#include "PlanillaReinicioEquipoEspecial.h"
#include "common.h"

PlanillaReinicioEquipoEspecial::PlanillaReinicioEquipoEspecial() {
    this->msgqReinicioKey = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_EQUIPO_ESPECIAL);
    this->msgqReinicioId = msgget(this->msgqReinicioKey, 0666);
    if(this->msgqReinicioId == -1) {
	std::string err = std::string("Error al obtener la cola de reinicio de tests de la planilla de reinicio del equipo especial. Errno: ") + std::string(strerror(errno));
	Logger::error(err, __FILE__);
        exit(1);
    }
}

PlanillaReinicioEquipoEspecial::~PlanillaReinicioEquipoEspecial() {
}

void PlanillaReinicioEquipoEspecial::avisarReinicio(const std::set<int>& idTesters, bool reiniciar) {

    TMessageAtendedor msg;
    msg.mtype = ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = MTYPE_HAY_QUE_REINICIAR;
    msg.hayQueReiniciar = reiniciar;
    for (std::set<int>::const_iterator it = idTesters.begin(); it != idTesters.end(); it++) {
        msg.tester = *it;
        std::stringstream ss;
        ss << "Se avisa del reinicio al tester especial " << *it;
        Logger::debug(ss.str(), __FILE__);
        int ret = msgsnd(this->msgqReinicioId, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
        if(ret == -1) {
            std::string error = std::string("Error al enviar reinicio de tests desde el equipo especial. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(0);
        }
    }
}