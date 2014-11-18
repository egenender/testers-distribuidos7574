#include <string>

#ifndef COMMON_H
#define	COMMON_H

// Constantes del sistema

const int CANT_DISPOSITIVOS = 20;
const int CANT_TESTERS = 5;
const int ID_DISPOSITIVO_START = 2;
const int ID_TESTER_START = 2;
const int MAX_DISPOSITIVOS = 20;


// IDs de los IPC
const int SEM_PLANILLA_GENERAL = 100;
const int SHM_PLANILLA_GENERAL = 200;
const int SEM_TESTER_A = 300;
const int SEM_TESTER_B = 400;
const int SEM_TESTER_RESULTADO = 500;
const int MSGQUEUE_PLANILLA = 600;
const int MSGQUEUE_ESCRITURA_RESULTADOS = 700;
const int MSGQUEUE_LECTURA_RESULTADOS = 800;
const int MSGQUEUE_NUEVO_REQUERIMIENTO = 900;
const int MSGQUEUE_DESPACHADOR = 10000;

const int CANTIDAD_TESTERS_MAXIMA = 5;
const int SEM_PLANILLA_LOCAL = 10;
const int SHM_PLANILLA_LOCAL = SEM_PLANILLA_LOCAL + CANTIDAD_TESTERS_MAXIMA;

const int MTYPE_REQUERIMIENTO = 1;
const int MTYPE_REQUERIMIENTO_SEGUNDO = 2;

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

const std::string ipcFileName = "/tmp/sorrentino-ipcs";

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

