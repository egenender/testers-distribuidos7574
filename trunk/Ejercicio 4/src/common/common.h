/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app
 */

#pragma once

#include <string>

namespace Constantes{

    namespace NombresDeParametros{
        const std::string ARCHIVO_IPCS = "ArchivoIpcs";
        const std::string CANT_DISPOSITIVOS = "CantDispositivos";
        const std::string CANT_TESTERS_ESP = "CantTestersEsp";
        const std::string ID_DISPOSITIVO_START = "IdDispositivoStart";
        const std::string ID_TESTER_ESP_START = "IdTesterEspStart";
        const std::string MAX_DISPOSITIVOS_EN_SISTEMA = "MaxDispositivosEnSistema";
        const std::string MAX_TIEMPO_ESPERA_REINTENTO_TESTER_OCUPADO = "MaxTiempoEsperaReintentoTesterOcupado";
        const std::string MIN_TIEMPO_ESPERA_REINTENTO_TESTER_OCUPADO = "MinTiempoEsperaReintentoTesterOcupado";
        const std::string MSG_QUEUE_ATENDEDOR = "MsqQueueAtendedor";
        const std::string MSG_QUEUE_DESPACHADOR = "MsqQueueDespachador";
        const std::string SEM_PLANILLA = "SemPlanilla";
        const std::string SHMEM_PLANILLA = "ShmPlanilla";
    }

    //Tipos de mensaje
    const int MTYPE_REQUERIMIENTO = 1001;

    const int ORDEN_APAGADO = 0;
    const int ORDEN_REINICIO = 1;

    const int RESULTADO_GRAVE = 0;
    const int RESULTADO_NO_GRAVE = 1;

    // Archivos necesarios
    const std::string ARCHIVO_LOG = "log.txt";

}

