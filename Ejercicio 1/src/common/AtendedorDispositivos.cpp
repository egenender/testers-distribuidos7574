/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */

#include "AtendedorDispositivos.h"

AtendedorDispositivos::AtendedorDispositivos() {
}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}

void AtendedorDispositivos::crearQueue() {}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {}
int AtendedorDispositivos::recibirRequerimiento() {}
void AtendedorDispositivos::enviarPrograma(int idPrograma) {}
int AtendedorDispositivos::recibirPrograma() {}
void AtendedorDispositivos::enviarResultado(int resultado) {}
int AtendedorDispositivos::recibirResultado() {}
void AtendedorDispositivos::enviarOrden(int orden) {}
int AtendedorDispositivos::recibirOrden() {}