#include <string>

#pragma once

namespace Constantes{
    
    namespace NombresDeParametros{
        const std::string ARCHIVO_IPCS = "/home/marcos/sorrentino-ipcs";
        const std::string CANT_DISPOSITIVOS = "CantDispositivos";
        const std::string CANT_TESTERS = "CantTesters";
        const std::string ID_DISPOSITIVO_START = "IdDispositivoStart";
        const std::string ID_TESTER_START = "IdTesterStart";
        const std::string MSGQUEUE_DESPACHADOR = "MsgQueueDespachador";
        const std::string MSGQUEUE_ESCRITURA_RESULTADOS = "MsgQueueEscrituraResultados";
        const std::string MSGQUEUE_LECTURA_RESULTADOS = "MsgQueueLecturaResultados";
        const std::string MSGQUEUE_NUEVO_REQUERIMIENTO = "MsgQueueNuevoRequerimiento";
        const std::string MSGQUEUE_PLANILLA = "MsgQueuePlanilla";
        const std::string SEM_PLANILLA_GENERAL = "SemPlanillaGeneral";
    }

    //Constantes del sistema
    const int MAX_DISPOSITIVOS = 20;

    // IDs de los IPC
    const int SHM_PLANILLA_GENERAL = 200;
    const int SEM_TESTER_A = 300;
    const int SEM_TESTER_B = 400;
    const int SEM_TESTER_RESULTADO = 500;    

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
    const std::string ARCHIVO_LOG = "log.txt";

}


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
