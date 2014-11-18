/* 
 * File:   TareaEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 11:53 PM
 */

#ifndef TAREAESPECIAL_H
#define	TAREAESPECIAL_H

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

#endif	/* TAREAESPECIAL_H */

