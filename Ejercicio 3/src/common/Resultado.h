/* 
 * File:   Resultado.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:28 PM
 */

#pragma once

#include "./common.h"

class Resultado {
public:
    Resultado();
    Resultado(const Resultado& orig);
    virtual ~Resultado();
    
    static bool esGrave(int resultado) { return (resultado == Constantes::RESULTADO_GRAVE); };
    
};
