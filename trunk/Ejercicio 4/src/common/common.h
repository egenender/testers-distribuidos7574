/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app, incluido nombre de archivos para IPCs
 */

#pragma once

#include <string>

namespace Constantes{

    //TODO pasar por argumento al lanzador o tomar de archivo para no tener que recompilar

    // Constantes del sistema
    const int CANT_DISPOSITIVOS = 1;
    const int ID_DISPOSITIVO_START = 1;

    //const int CANT_TESTERS = 1;
    const int CANT_TESTERS_ESP = 4;

    //const int ID_TESTER_START = 2;
    const int ID_TESTER_ESP_START = 1;
    const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;

    // IDs de los IPC
    const int MSG_QUEUE_ATENDEDOR = 10;
    const int SEM_PLANILLA = 20;
    const int SHMEM_PLANILLA = 30;
    const int MSG_QUEUE_DESPACHADOR = 40;

    //Tipos de mensaje
    const int MTYPE_REQUERIMIENTO = 1001;

    const int ORDEN_APAGADO = 0;
    const int ORDEN_REINICIO = 1;

    const int RESULTADO_GRAVE = 0;
    const int RESULTADO_NO_GRAVE = 1;

    // Archivos necesarios
    const std::string ARCHIVO_IPCS = "/home/dario/ramos-ipcs";
    const std::string ARCHIVO_LOG = "log.txt";

}

