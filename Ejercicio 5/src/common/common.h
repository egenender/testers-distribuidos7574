/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app, incluido nombre de archivos para IPCs
 */

#include <string>

#ifndef COMMON_H
#define	COMMON_H

// Constantes del sistema

const int CANT_DISPOSITIVOS = 20;
const int CANT_TESTERS = 5;
const int ID_DISPOSITIVO_START = 2;
const int ID_TESTER_START = 2;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;

// IDs de los IPC

const int MSG_QUEUE_ATENDEDOR = 0;
const int SEM_PLANILLA = 1;
const int SHMEM_PLANILLA = 2;
const int MSG_QUEUE_DESPACHADOR = 3;

const int MTYPE_REQUERIMIENTO = 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;

const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;

// Archivos necesarios

const std::string ipcFileName = "/tmp/pereira-ipcs";

const std::string logFileName = "log.txt";


#endif	/* COMMON_H */

