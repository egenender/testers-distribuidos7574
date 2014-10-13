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
    
    while(1) {
        // Dispositivo envia requerimiento
        atendedor.enviarRequerimiento(id);
        // Recibe programa, verificando que no sea un rechazo por parte del sistema
        int program = atendedor.recibirPrograma(id);
        if (program == -1) {
            // Si no hay programa -> no hay lugar -> Duermo y envio otro req mas tarde
            sleep(rand() % 60 + 60);
            continue;
        }
    
        // Le envio resultado del programa de testeo
        atendedor.enviarResultado(id, rand() % 2);
        
        // Recibo la orden a seguir
        int orden = atendedor.recibirOrden(id);
        
        if (orden == ORDEN_APAGADO) {
            Logger::notice("Al dispositivo le ha llegado una orden de apagado", __FILE__);
            break;
        } else {
            Logger::notice("Al dispositivo le ha llegado una orden de reinicio", __FILE__);
            break;
        }
    }
    
    Logger::notice("El dispositivo ha terminado el testeo", __FILE__);
    
    Logger::destroy();
    
    return 0;
}

