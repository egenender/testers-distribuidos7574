/* 
 * File:   TareaEspecial.h
 * Author: knoppix
 *
 * Created on November 16, 2014, 11:53 PM
 */

#pragma once

#include <list>
#include <cstdlib>

#include "common/common.h"

class TareaEspecial {
public:
    TareaEspecial();
    virtual ~TareaEspecial();
    
    void prepararTareasEspeciales();
    int getProximaTareaEspecial();
    int getCantTareasEspeciales();
    void resetTareas();
private:
    std::list<int> tareas;
    std::list<int>::iterator currentTarea;
    int cantTareas;
};
