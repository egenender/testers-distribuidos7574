/* 
 * File:   PlanillaReinicioEquipoEspecial.h
 * Author: ferno
 *
 * Created on November 17, 2014, 1:35 PM
 */

#ifndef PLANILLAREINICIOEQUIPOESPECIAL_H
#define	PLANILLAREINICIOEQUIPOESPECIAL_H

#include <set>
#include <cstdlib>
#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "logger/Logger.h"

class PlanillaReinicioEquipoEspecial {
public:
    PlanillaReinicioEquipoEspecial();
    virtual ~PlanillaReinicioEquipoEspecial();
    
    void avisarReinicio(const std::set<int>& idTesters, bool reiniciar);
    
private:
    key_t msgqReinicioKey;
    int msgqReinicioId;

};

#endif	/* PLANILLAREINICIOEQUIPOESPECIAL_H */

