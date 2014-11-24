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

const int CANT_DISPOSITIVOS = 300;
const int CANT_TESTERS = 5;
const int ID_DISPOSITIVO_START = 50;
const int ID_TESTER_START = 1;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;

const int MINIMOS_LANZADOS = 1;
const int MAXIMOS_LANZADOS = 15;

// IDs de los IPC
const int SEM_PLANILLA_GENERAL = 1;
const int SHM_PLANILLA_GENERAL = 2;
const int SEM_TESTER_A = 30;
const int SEM_TESTER_B = 40;
const int MSGQUEUE_ESCRITURA_RESULTADOS = 80;
const int MSGQUEUE_LECTURA_RESULTADOS = 81;
const int MSGQUEUE_NUEVO_REQUERIMIENTO = 82;
const int MSGQUEUE_ORDENES = 83;
const int MSGQUEUE_DESPACHADOR = 84;
const int MSGQUEUE_PLANILLA = 85;
const int TIPO_A = 0;
const int TIPO_B = 1;

const int CANTIDAD_TESTERS_MAXIMA = 10;
const int SEM_PLANILLA_LOCAL = 10;
const int SHM_PLANILLA_LOCAL = SEM_PLANILLA_LOCAL + CANTIDAD_TESTERS_MAXIMA;

const int MTYPE_REQUERIMIENTO = 1;

const int ORDEN_APAGADO = 0;
const int ORDEN_REINICIO = 1;
const int SIN_LUGAR = -1;

const int RESULTADO_GRAVE = 0;
const int RESULTADO_NO_GRAVE = 1;


const int REQUERIMIENTO_AGREGAR = 0;
const int REQUERIMIENTO_TERMINO_PENDIENTE_REQ = 1;
const int REQUERIMIENTO_ELIMINAR_DISPOSITIVO = 2;
const int REQUERIMIENTO_INICIAR_PROC_RESULTADOS = 3;
const int REQUERIMIENTO_PROCESAR_SIGUIENTE = 4;

// Archivos necesarios

const std::string ipcFileName = "/tmp/buchwaldipcs";

const std::string logFileName = "log.txt";


//Estructuras communes:
typedef struct requerimiento_planilla{
    long tester;
    int tipoReq;
    int idDispositivo;
}requerimiento_planilla_t;

typedef struct respuesta_lugar{
    long mtype;
    bool respuesta;
}respuesta_lugar_t;

typedef struct resultado_test{
    long tester;
    int result;
    int dispositivo;
}resultado_test_t;

#endif	/* COMMON_H */

