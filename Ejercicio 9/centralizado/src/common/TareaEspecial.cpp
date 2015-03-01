/* 
 * File:   TareaEspecial.cpp
 * Author: knoppix
 * 
 * Created on November 16, 2014, 11:53 PM
 */

#include "TareaEspecial.h"
#include "Configuracion.h"

using namespace Constantes::NombresDeParametros;

TareaEspecial::TareaEspecial( const Configuracion& config ) :
        m_MaxDispositivosEnSistema( config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA) ),
        m_MaxTareasEspeciales( config.ObtenerParametroEntero(MAX_TAREAS_ESPECIALES) ){
    this->cantTareas = 0;
    
}

TareaEspecial::~TareaEspecial() {
}

// Creo una lista de tareas random
void TareaEspecial::prepararTareasEspeciales() {
    this->cantTareas = (rand() % m_MaxTareasEspeciales) + 1;
    for (int i = 0; i < cantTareas; i++) {
        tareas.push_back((rand() % (m_MaxDispositivosEnSistema)) + 500);
    }
    this->currentTarea = tareas.begin();
}

int TareaEspecial::getProximaTareaEspecial() {
    if (this->currentTarea == tareas.end()) {
        return -1;
    }
    int tarea =  (*this->currentTarea);
    this->currentTarea++;
    return tarea;
}

int TareaEspecial::getCantTareasEspeciales() {
    return this->cantTareas;
}

void TareaEspecial::resetTareas() {
    this->currentTarea = this->tareas.begin();
}
