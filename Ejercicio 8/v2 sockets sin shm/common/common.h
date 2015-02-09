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
const int CANT_TESTERS_COMUNES = 5;
const int CANT_TESTERS_ESPECIALES = 15;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;
const int MAX_TAREAS_ESPECIALES = 10;

static int ID_DISPOSITIVO = 5;
const int MAX_TESTER_COMUNES = 30;
const int MAX_TESTER_ESPECIALES = 30;
const int ID_TESTER_COMUN_START = ID_DISPOSITIVO + CANT_DISPOSITIVOS + 1;
const int ID_TESTER_ESP_START = ID_TESTER_COMUN_START + MAX_TESTER_COMUNES + 1;
const int ID_EQUIPO_ESPECIAL = ID_TESTER_ESP_START + MAX_TESTER_ESPECIALES + 1;

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

const int SEM_TABLA_TESTERS_COMUNES_DISPONIBLES = 11;
const int SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES = 12;

const int SEM_IDENTIFICADOR = 13;

const int MSGQUEUE_ENVIO_DISP = 20;
const int MSGQUEUE_RECEPCIONES_DISP = 21;
const int MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS = 22;
const int MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS = 23;
const int MSGQUEUE_BROKER_RECEPTOR = 24;
const int MSGQUEUE_BROKER_EMISOR = 25;
const int MSGQUEUE_ENVIO_TESTER_COMUN = 26;
const int MSGQUEUE_RECEPCIONES_TESTER_COMUN = 27;
const int MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN = 28;
const int MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN = 29;
const int MSGQUEUE_ENVIO_TESTER_ESPECIAL = 30;
const int MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL = 31;
const int MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL = 32;
const int MSGQUEUE_ENVIO_EQUIPO_ESPECIAL = 33;
const int MSGQUEUE_DESPACHADOR = 34;
const int MSGQUEUE_REINICIO_TESTEO = 35;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS = 36;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL = 37;

const int LAST_ID_IPC = MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL + 1;
const int SEM_ESPECIALES = LAST_ID_IPC; // Semaforos para testers especiales (creciente)

// mtypes desde el dispositivo
const int MTYPE_REQUERIMIENTO_DISPOSITIVO = 1;
const int MTYPE_RESULTADO_INICIAL = 2;
const int MTYPE_RESULTADO_ESPECIAL = 3;
// mtypes desde tester comun
const int MTYPE_PROGRAMA_INICIAL = 1;
const int MTYPE_REQUERIMIENTO_TESTER_ESPECIAL = 4;
// mtypes desde tester especial
const int MTYPE_TAREA_ESPECIAL = 1;
// mtypes desde equipo especial
const int MTYPE_ORDEN = 1;
const int MTYPE_FIN_TEST_ESPECIAL = 5;

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

//const int MAXIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 4;
//const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;
// Archivos necesarios

const std::string ipcFileName = "/tmp/pereira-ipcs";

const std::string logFileName = "log.txt";

// Para los sockets
const char PUERTO_SERVER_RECEPTOR_DISPOSITIVOS[] = "50000";
const char PUERTO_SERVER_EMISOR_DISPOSITIVOS[] = "50001";

const char PUERTO_SERVER_RECEPTOR[] = "50002";
const char PUERTO_SERVER_EMISOR[] = "50003";

const char UBICACION_SERVER[] = "localhost"; //Cambiar
const char UBICACION_SERVER_IDENTIFICADOR[] = "localhost";

//Estructuras communes:
typedef struct resultado{
	int idDispositivo;
	int resultadosPendientes;
	int resultadosGraves;
}resultado_t;

typedef struct TContadorTesterEspecial {
    int cantTestersEspecialesTotal;
    int cantTestersEspecialesTerminados;
} TContadorTesterEspecial;

typedef struct TContadorTareaEspecial {
    int cantTareasEspecialesTotal;
    int cantTareasEspecialesTerminadas;
} TContadorTareaEspecial;

typedef struct TMessageReinicioTest {
    long mtype;
    bool hayQueReiniciar;
} TMessageReinicioTest;

// Primer mensaje de protocolo que cliente receptor le envia al server emisor
typedef struct TFirstMessage {
    int identificador;
} TFirstMessage;

typedef struct message {
	/* BEGIN HEADER */
	long mtype;
	long mtypeMensaje;
	int idDispositivo;
	int tester;
	int value; // Este parametro posee el valor del programa, del resultado y de la orden
	int posicionDispositivo;
} TMessageAtendedor;

typedef struct TTablaIdTestersDisponibles {
    bool disponibles[MAX_TESTER_COMUNES];
    int ultimoTesterElegido;
} TTablaIdTestersDisponibles;

typedef struct TTablaIdTestersEspecialesDisponibles {
    bool disponibles[MAX_TESTER_ESPECIALES];
    int ultimoTesterElegido;
} TTablaIdTestersEspecialesDisponibles;

#endif	/* COMMON_H */

