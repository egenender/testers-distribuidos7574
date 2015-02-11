/* 
 * File:   PlanillaReinicioTesterEspecial.cpp
 * Author: ferno
 * 
 * Created on November 17, 2014, 12:19 AM
 */

#include "PlanillaReinicioTesterEspecial.h"

PlanillaReinicioTesterEspecial::PlanillaReinicioTesterEspecial() {
    this->msgqReinicioKey = ftok(ipcFileName.c_str(), MSGQUEUE_REINICIO_TESTEO);
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

    TMessageReinicioTest msg;
    int ret = msgrcv(this->msgqReinicioId, &msg, sizeof(TMessageReinicioTest) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al recibir reinicio de testeo para tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.hayQueReiniciar;
}