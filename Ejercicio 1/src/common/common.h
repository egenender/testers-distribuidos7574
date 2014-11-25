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

const int CANT_DISPOSITIVOS = 150;
const int CANT_TESTERS = 5;
const int ID_DISPOSITIVO_START = 10;
const int ID_TESTER_START = ID_DISPOSITIVO_START + CANT_DISPOSITIVOS;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;

// IDs de los IPC

const int MSG_QUEUE_ATENDEDOR = 10;
const int SEM_PLANILLA = 20;
const int SHMEM_PLANILLA = 30;
const int MSG_QUEUE_DESPACHADOR = 40;

const int MTYPE_REQUERIMIENTO = 1;
const int MTYPE_ORDEN = 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;

const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;

// Archivos necesarios

const std::string ipcFileName = "/tmp/pereira-ipcs";

const std::string logFileName = "log.txt";

typedef struct message {
    long mtype;
    int idDispositivo;
    int idTester;
    int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
} TMessageAtendedor;

#endif	/* COMMON_H */

