/* 
 * File:   TareaEspecial.h
 * Author: ferno
 *
 * Created on November 16, 2014, 11:53 PM
 */

#pragma once

#include <list>
#include <cstdlib>

#include "common/common.h"

class Configuracion;

class TareaEspecial {
public:
    TareaEspecial( const Configuracion& config );
    ~TareaEspecial();

    void prepararTareasEspeciales();
    int getProximaTareaEspecial();
    int getCantTareasEspeciales();
    void resetTareas();
private:
    const int m_MaxDispositivosEnSistema;
    const int m_MaxTareasEspeciales;
    std::list<int> tareas;
    std::list<int>::iterator currentTarea;
    int cantTareas;
};
