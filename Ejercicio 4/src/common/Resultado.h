/* 
 * File:   Resultado.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:28 PM
 */

#pragma once

#include "common/common.h"

class Resultado {

//Prohibo copia y asignacion
    Resultado(const Resultado& orig);
    Resultado& operator=(const Resultado& rv);

public:
    Resultado();

    ~Resultado();

    static int esGrave(int resultado) {return (resultado == Constantes::RESULTADO_GRAVE);}; //TODO Violacion de OOP
};

