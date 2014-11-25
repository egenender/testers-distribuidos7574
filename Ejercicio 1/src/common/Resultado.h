/* 
 * File:   Resultado.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:28 PM
 */

#ifndef RESULTADO_H
#define	RESULTADO_H

#include "common/common.h"

class Resultado {
public:
    Resultado();
    Resultado(const Resultado& orig);
    virtual ~Resultado();
    
    static bool esGrave(int resultado) {return (resultado == RESULTADO_GRAVE);};
    
};

#endif	/* RESULTADO_H */

