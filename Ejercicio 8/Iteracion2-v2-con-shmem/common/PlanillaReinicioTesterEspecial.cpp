/* 
 * File:   PlanillaReinicioTesterEspecial.cpp
 * Author: ferno
 * 
 * Created on November 17, 2014, 12:19 AM
 */

#include "PlanillaReinicioTesterEspecial.h"

PlanillaReinicioTesterEspecial::PlanillaReinicioTesterEspecial() {
    this->msgqReinicioKey = ftok(ipcFileName.c_str(), MSGQUEUE_REINICIO_TESTER_ESPECIAL);
    this->msgqReinicioId = msgget(this->msgqReinicioKey, 0666);
    if(this->msgqReinicioId == -1) {
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
    int ret = msgrcv(this->msgqReinicioId, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
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