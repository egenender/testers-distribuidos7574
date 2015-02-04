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

const int CANT_DISPOSITIVOS = 101;
const int CANT_TESTERS_COMUNES = 5;
const int CANT_TESTERS_ESPECIALES = 15;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;
const int MAX_TAREAS_ESPECIALES = 10;

const int MINIMOS_LANZADOS = 5;
const int MAXIMOS_LANZADOS = 20;

// IDs de los IPC
const int SEM_PLANILLA_GENERAL = 1;
const int SHM_PLANILLA_GENERAL = 2;
const int SEM_COLA_ESPECIALES = 3;
const int SEM_PLANILLA_CANT_TESTER_ASIGNADOS = 4;
const int SEM_PLANILLA_CANT_TAREAS_ASIGNADAS = 5;
const int SHM_PLANILLA_CANT_TESTER_ASIGNADOS = 6;
const int SHM_PLANILLA_CANT_TAREAS_ASIGNADAS = 7;
const int SHM_PLANILLA_GENERAL_POSICIONES = 8;

const int SHM_TESTERS_COMUNES_DISPONIBLES = 9;
const int SHM_TESTERS_ESPECIALES_DISPONIBLES = 10;

//Confirmado que si va
const int MSGQUEUE_ENVIOS_DISP = 20;
const int MSGQUEUE_RECEPCIONES_DISP = 21;
const int MSGQUEUE_ENVIOS_TESTER = 22;
const int MSGQUEUE_RECEPCIONES_TESTER = 23;
const int MSGQUEUE_TESTERS_ESPECIALES = 24;

//en duda
const int MSGQUEUE_DESPACHADOR = 23;
const int MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES = 24;
const int MSGQUEUE_REINICIO_TESTEO = 25;
const int MSGQUEUE_ULTIMO = MSGQUEUE_REINICIO_TESTEO;

const int MTYPE_REQUERIMIENTO = 1;
const int MTYPE_RESULTADO_ESPECIAL = 1;
const int MTYPE_ORDEN = 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;
const int ORDEN_SEGUIR_TESTEANDO = 2;
const int SIN_LUGAR = -1;

const int CANT_RESULTADOS = MAX_DISPOSITIVOS_EN_SISTEMA;
const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;
const int SEGUIR_TESTEANDO = 2;
const int NO_CONTESTAR = 3;
const int FIN_TEST_ESPECIAL = 4;

const int MAXIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 4;
const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;
// Archivos necesarios

const std::string ipcFileName = "/tmp/pereira-ipcs";

const std::string logFileName = "log.txt";

// Para los sockets
const char PUERTO_SERVER_RECEPTOR_DISP[] = "9003";
const char PUERTO_SERVER_EMISOR_DISP[] = "9002";

const char PUERTO_SERVER_RECEPTOR_TESTERS[] = "9000";
const char PUERTO_SERVER_EMISOR_TESTERS[] = "9001";

const char UBICACION_SERVER[] = "localhost"; //Cambiar
const char UBICACION_SERVER_IDENTIFICADOR[] = "localhost";

/*
 *      BROKER COMMON
 */

const int MAX_TESTERS_COMUNES = 100;
const int MAX_TESTERS_ESPECIALES = 100;

//Estructuras communes:
typedef struct resultado{
	int idDispositivo;
	int resultadosPendientes;
	int resultadosGraves;
}resultado_t;

typedef struct TMessageAssignTE{
	long mtype;
	int idDispositivo;
    int posicionDispositivo;
} TMessageAssignTE;

typedef struct resultado_test{
    long mtype;
    int idDispositivo;
    int result;
}resultado_test_t;

typedef struct TContadorTesterEspecial {
    int cantTestersEspecialesTotal;
    int cantTestersEspecialesTerminados;
} TContadorTesterEspecial;

typedef struct TContadorTareaEspecial {
    int cantTareasEspecialesTotal;
    int cantTareasEspecialesTerminadas;
} TContadorTareaEspecial;

typedef struct TResultadoEspecial {
    long mtype;
    int idDispositivo;
    int idTester;
    int posicionDispositivo;
    int resultado;
} TResultadoEspecial;

typedef struct TMessageReinicioTest {
    long mtype;
    bool hayQueReiniciar;
} TMessageReinicioTest;

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
	int posicionDispositivo;
	int especiales[MAX_TESTERS_ESPECIALES];
} TMessageAtendedor;

#endif	/* COMMON_H */

