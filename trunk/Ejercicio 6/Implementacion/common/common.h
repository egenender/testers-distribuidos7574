/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app, incluido nombre de archivos para IPCs
 */

#include <string>
#include <stddef.h>

#ifndef COMMON_H
#define	COMMON_H

// Constantes del sistema

const int CANT_DISPOSITIVOS = 100;
const int CANT_TESTERS_COMUNES = 3;
const int CANT_TESTERS_ESPECIALES = 5;
const int ID_DISPOSITIVO_START = 50;
const int ID_TESTER_START = 1;
const int ID_TESTER_ESPECIAL_START = ID_TESTER_START + CANT_TESTERS_COMUNES;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;


// IDs de los IPC
const int SEM_PLANILLA_GENERAL = 1;
const int SHM_PLANILLA_GENERAL = 2;

const int MSGQUEUE_DISPOSITIVOS = 10;
const int MSGQUEUE_TESTERS = 11;
const int MSGQUEUE_TESTERS_ESPECIALES = 12;
const int MSGQUEUE_DESPACHADOR = 13;

const int MTYPE_REQUERIMIENTO = 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;
const int ORDEN_SEGUIR_TESTEANDO = 2;
const int SIN_LUGAR = -1;

const int CANT_RESULTADOS = 100;
const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;
const int SEGUIR_TESTEANDO = 2;
const int NO_CONTESTAR = 3;

const int MAXIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 4;
const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;
// Archivos necesarios

const std::string ipcFileName = "tmp/buchwaldipcs";

const std::string logFileName = "log.txt";


//Estructuras communes:
typedef struct resultado{
	int idDispositivo;
	int resultadosPendientes;
	int resultadosGraves;
}resultado_t;

typedef struct posicion_en_shm{
	long mtype;
	int lugar;
}posicion_en_shm_t;

typedef struct resultado_test{
    long tester;
    int result;
    int dispositivo;
}resultado_test_t;

#endif	/* COMMON_H */

