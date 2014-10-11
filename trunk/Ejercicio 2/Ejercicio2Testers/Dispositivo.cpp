/* 
 * File:   Dispositivo.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:12 PM
 * 
 * El dispositivo interactua con el sistema de testeo, enviando requerimientos, recibiendo programas, enviando resultados
 * y esperando ordenes
 */

#include <cstdlib>

#include "common/AtendedorDispositivos.h"
#include "logger/Logger.h"
#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // Por parametro se recibe el ID del dispositivo
    int id = atoi(argv[1]);
    
    // Comunicacion con el sistema de testeo
    AtendedorDispositivos atendedor;
    
    // TODO: Log
    
    while(true) {
        // Dispositivo envia requerimiento
        atendedor.enviarRequerimiento(id);
        // Recibe programa, verificando que no sea un rechazo por parte del sistema
        int program = atendedor.recibirPrograma(id);
        if (program == SIN_LUGAR) {
            // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
            sleep(rand() % 60 + 60);
            continue;
        }
    
        // Le envio resultado del programa de testeo
        atendedor.enviarResultado(id, rand() % 2);
        
        // Recibo la orden a seguir
        int orden = atendedor.recibirOrden(id);
        
        if (orden == ORDEN_APAGADO) {
            // TODO: Log
            break;
        } else {
            // TODO: Log
            sleep(rand()%60 + 180);
        }
    }
    
    // TODO: Log
    
    return 0;
}

