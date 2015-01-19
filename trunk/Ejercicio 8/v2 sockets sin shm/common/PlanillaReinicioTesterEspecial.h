/* 
 * File:   PlanillaReinicioTesterEspecial.h
 * Author: ferno
 *
 * Created on November 17, 2014, 12:19 AM
 */

#ifndef PLANILLAREINICIOTESTERESPECIAL_H
#define	PLANILLAREINICIOTESTERESPECIAL_H

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"

#include "common.h"
#include "logger/Logger.h"

class PlanillaReinicioTesterEspecial {
public:
    PlanillaReinicioTesterEspecial();
    virtual ~PlanillaReinicioTesterEspecial();
    
    bool hayQueReiniciar(int idTester);
private:
    key_t msgqReinicioKey;
    int msgqReinicioId;

};

#endif	/* PLANILLAREINICIOTESTERESPECIAL_H */

