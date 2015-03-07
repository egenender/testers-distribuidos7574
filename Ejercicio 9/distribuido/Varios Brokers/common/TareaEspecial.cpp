/* 
 * File:   TareaEspecial.cpp
 * Author: ferno
 * 
 * Created on November 16, 2014, 11:53 PM
 */

#include "TareaEspecial.h"

TareaEspecial::TareaEspecial() {
    this->cantTareas = 0;
}

TareaEspecial::~TareaEspecial() {
}

// Creo una lista de tareas random
void TareaEspecial::prepararTareasEspeciales() {
    this->cantTareas = (rand() % MAX_TAREAS_ESPECIALES) + 1;
    for (int i = 0; i < cantTareas; i++) {
        tareas.push_back((rand() % (MAX_DISPOSITIVOS_EN_SISTEMA)) + 500);
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