/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app, incluido nombre de archivos para IPCs
 */

#ifndef COMMON_H
#define	COMMON_H

#include <stddef.h>

#ifndef SOLO_C
#include "commonFiles.h"
#endif

// Constantes del sistema

const int CANT_DISPOSITIVOS = 100;
const int CANT_TESTERS_COMUNES = 3;
const int CANT_TESTERS_ESPECIALES = 5;
const int ID_DISPOSITIVO_START = 50;
const int ID_TESTER_START = 1;

#define MAX_DISPOSITIVOS_EN_SISTEMA 100
#define MAX_TESTERS_COMUNES  10
#define MAX_TESTERS_ESPECIALES 10

const int ID_TESTER_ESPECIAL_START = MAX_TESTERS_COMUNES;

const int TIPO_COMUN = 0;
const int TIPO_ESPECIAL = 1;
const int TIPO_DISPOSITIVO = 2;

// IDs de los IPC
const int SEM_PLANILLA_GENERAL = 1;
const int SHM_PLANILLA_GENERAL = 2;
const int SEM_COLA_ESPECIALES = 3;

const int MSGQUEUE_DISPOSITIVOS_ENVIOS = 10;
const int MSGQUEUE_DISPOSITIVOS_RECIBOS = 11;
const int MSGQUEUE_TESTERS_ENVIOS = 12;
const int MSGQUEUE_TESTERS_RECIBOS = 13;
const int MSGQUEUE_TESTERS_ESPECIALES = 14;
const int MSGQUEUE_DESPACHADOR = 15;
const int MSGQUEUE_SERVER_RECEPTOR_EMISOR = 16;
const int MSGQUEUE_GETTING_IDS = 17;

const int MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS = 18;
const int MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS = 19;
const int MSGQUEUE_BROKER_ATENCION_REQUERIMIENTOS_DISPOSITIVOS = 20;
const int MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS = 21;	
const int SEM_CANT_TESTERS_COMUNES = 22;
const int SEM_TABLA_TESTERS = 23;
const int SHM_TABLA_TESTERS = 24;
const int SEM_ESPECIAL_DISPONIBLE = 25; //siguiente debe ser + CANT_ESPECIALes

// CONSTANTES DEL MANEJO DEL SISTEMA

const int MTYPE_REQUERIMIENTO = MAX_TESTERS_COMUNES + MAX_TESTERS_ESPECIALES + 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;
const int ORDEN_SEGUIR_TESTEANDO = 2;
const int SIN_LUGAR = -1;

const int CANT_RESULTADOS = MAX_DISPOSITIVOS_EN_SISTEMA;
const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;
const int SEGUIR_TESTEANDO = 2;
const int NO_CONTESTAR = 3;
const int FINALIZAR_CONEXION = 1;

const int MAXIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 4;
const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;

const char PUERTO_SERVER_RECEPTOR_TESTERS[] = "9000";
const char PUERTO_SERVER_EMISOR_TESTERS[] = "9001";
const char PUERTO_SERVER_RECEPTOR_DISPOSITIVOS[] = "9002";
const char PUERTO_SERVER_EMISOR_DISPOSITIVOS[] = "9003";

const char UBICACION_SERVER[] = "localhost"; //Cambiar

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

typedef struct message {
	/* BEGIN HEADER */
	long mtype;
	long mtype_envio;
	int finalizar_conexion;
	int es_requerimiento;
	int es_especial; //Solo por si acaso
	/* END HEADER */
	int idDispositivo;
	int tester;
	int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
	int cant_testers;
	int especiales[MAX_TESTERS_ESPECIALES];
} TMessageAtendedor;

typedef struct tabla_testers_disponibles{
	int testers_comunes[MAX_TESTERS_COMUNES];
	int start;
	int end;
	int cant;
	int testers_especiales[MAX_TESTERS_ESPECIALES];
	int dispositivos_atendidos; //por ahora no se usa
} tabla_testers_disponibles_t;

#endif	/* COMMON_H */
