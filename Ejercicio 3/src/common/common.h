#include <string>

#pragma once

namespace Constantes{
    
    namespace NombresDeParametros{
        const std::string ARCHIVO_IPCS = "ArchivoIpcs";
        const std::string CANT_DISPOSITIVOS = "CantDispositivos";
        const std::string CANT_TESTERS = "CantTesters";
        const std::string ID_DISPOSITIVO_START = "IdDispositivoStart";
        const std::string ID_TESTER_START = "IdTesterStart";        
        const std::string MSGQUEUE_DESPACHADOR = "MsgQueueDespachador";
        const std::string MSGQUEUE_ESCRITURA_RESULTADOS = "MsgQueueEscrituraResultados";
        const std::string MSGQUEUE_LECTURA_RESULTADOS = "MsgQueueLecturaResultados";
        const std::string MSGQUEUE_NUEVO_REQUERIMIENTO = "MsgQueueNuevoRequerimiento";
        const std::string MSGQUEUE_PLANILLA = "MsgQueuePlanilla";
        const std::string SEM_PLANILLA_LOCAL = "SemPlanillaLocal";
        const std::string SEM_PLANILLA_GENERAL = "SemPlanillaGeneral";
        const std::string SEM_TESTER_A = "SemTesterA";
        const std::string SEM_TESTER_B = "SemTesterB";
        const std::string SEM_TESTER_RESULTADO = "SemTesterResultado";
        const std::string SHM_PLANILLA_GENERAL = "ShmPlanillaGeneral";
        const std::string SHM_PLANILLA_LOCAL = "ShmPlanillaLocal";
        const std::string TESTER_ID_OFFSET = "TesterIdOffset";
        const std::string TESTER_2DO_ID_OFFSET = "Tester2doIdOffset";
        const std::string TESTER_RTA_ID_OFFSET = "TesterRtaIdOffset";

    }

    //Constantes del sistema
    

    // IDs de los IPC
    const int CANTIDAD_TESTERS_MAXIMA = 5;
    const int MAX_DISPOSITIVOS_POR_TESTER = 20;

    const int MTYPE_REQUERIMIENTO = 4000;
    const int MTYPE_REQUERIMIENTO_SEGUNDO = 5000;

    const int ORDEN_APAGADO = 0;
    const int ORDEN_REINICIO = 1;
    const int SIN_LUGAR = -1;

    const long OFFSET_LUGAR = 15000;
    
    const int RESULTADO_GRAVE = 0;
    const int RESULTADO_NO_GRAVE = 1;

    const int REQUERIMIENTO_AGREGAR = 0;
    const int REQUERIMIENTO_TERMINO_PENDIENTE_REQ = 6000;
    const int REQUERIMIENTO_ELIMINAR_DISPOSITIVO = 7000;
    const int REQUERIMIENTO_INICIAR_PROC_RESULTADOS = 8000;
    const int REQUERIMIENTO_PROCESAR_SIGUIENTE = 9000;
    const int REQUERIMIENTO_INICIAR_PROC_RESULTADOS_PARCIALES = 10000;
    const int REQUERIMIENTO_AGREGAR_RESULTADO_PARCIAL = 11000;
    const int REQUERIMIENTO_AGREGAR_RESULTADO = 12000;

    // Archivos necesarios
    const std::string ARCHIVO_LOG = "log.txt";
}


//Estructuras communes:
typedef struct requerimiento_planilla{
    long mtype;
    int tester;
    int tipoReq;
    int idDispositivo;
}requerimiento_planilla_t;

typedef struct respuesta_lugar{
    long mtype;
    bool respuesta;
}respuesta_lugar_t;

enum TipoResultado{
    PARCIAL = 0,
    FINAL
};

typedef struct resultado_test{
    long tester;
    int result;
    int dispositivo;
    TipoResultado tipo;
}resultado_test_t;

typedef struct message {
    long mtype;
    int idDispositivo;
    int idTester;
    int requerimiento;
    int programa;
    int resultado;
    int orden;
    TipoResultado tipoResultado;
} TMessageAtendedor;
