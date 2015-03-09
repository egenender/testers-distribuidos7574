/* 
 * File:   common.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:13 PM
 * 
 * Aqui se almacenan las constantes correspondientes a toda la app, incluido nombre de archivos para IPCs
 */

extern "C"{
    #include "common_c.h"
}
#include <string>
#include <stddef.h>

#pragma once

namespace Constantes{
    namespace NombresDeParametros{const std::string ARCHIVO_IPCS = "ArchivoIpcs";
        const std::string CANT_TESTERS_ESPECIALES = "CantTestersEspeciales";
        const std::string MSGQUEUE_DESPACHADOR = "MsgQueueDespachador";
        const std::string MSGQUEUE_ENVIO_DISP = "MsgQueueEnvioDisp";
        const std::string MSGQUEUE_ENVIO_EQUIPO_ESPECIAL = "MsgQueueEnvioEquipoEspecial";
        const std::string MSGQUEUE_ENVIO_TESTER_COMUN = "MsgQueueEnvioTesterComun";
        const std::string MSGQUEUE_ENVIO_TESTER_ESPECIAL = "MsgQueueEnvioTesterEspecial";
        const std::string MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION = "MsgQueueEnvioTestersShmPlanillaAsignacion";
        const std::string MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION = "MsgQueueRecepcionTestersShmemPlanillaAsignacion";
        const std::string MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL = "MsgQueueEnvioTestersShmemPlanillaGeneral";
        const std::string MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL = "MsgQueueRecepcionTestersShmPlanillaGeneral";
        const std::string MSGQUEUE_RECEPCIONES_DISP = "MsgQueueRecepcionesDisp";
        const std::string MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL = "MsgQueueRecepcionesEquipoEspecial";
        const std::string MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN = "MsgQueueRecepcionesNoReqTesterComun";
        const std::string MSGQUEUE_RECEPCIONES_TESTER_COMUN = "MsgQueueRecepcionesTesterComun";
        const std::string MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL = "MsgQueueRecepcionesTesterEspecial";
        const std::string MSGQUEUE_REINICIO_TESTER_ESPECIAL = "MsqQueueReinicioTesterEspecial";
        const std::string MSGQUEUE_REQ_TESTER_ESPECIAL = "MsgQueueReqTesterEspecial";
        const std::string MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS = "MsgQueueReqTestersShmemPlanillas";
        const std::string MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN = "MsgQueueRequerimientosTesterComun";
        const std::string PUERTO_SERVER_EMISOR = "PuertoServerEmisor";
        const std::string PUERTO_SERVER_EMISOR_DISPOSITIVOS = "PuertoServerEmisorDispositivos";
        const std::string PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION = "PuertoServerEnvioShmPlanillaAsignacion";
        const std::string PUERTO_SERVER_ENVIO_SHM_PLANILLA_GENERAL = "PuertoServerEnvioShmPlanillaGeneral";
        const std::string PUERTO_SERVER_RECEPCION_REQ_SHM = "PuertoServerRecepcionReqShm";
        const std::string PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION = "PuertoServerRecepcionShmPlanillaAsignacion";
        const std::string PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL = "PuertoServerRecepcionShmPlanillaGeneral";
        const std::string PUERTO_SERVER_RECEPTOR = "PuertoServerReceptor";
        const std::string PUERTO_SERVER_RECEPTOR_DISPOSITIVOS = "PuertoServerReceptorDispositivos";
        const std::string SEM_IDENTIFICADOR = "SemIdentificador";
        const std::string SEM_TABLA_TESTERS_COMUNES_DISPONIBLES = "SemTablaTestersComunesDisponibles";
        const std::string SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES = "SemTablaTestersEspecialesDisponibles";
        const std::string SHM_TESTERS_COMUNES_DISPONIBLES = "ShmTestersComunesDisponibles";
        const std::string SHM_TESTERS_ESPECIALES_DISPONIBLES = "ShmTestersEspecialesDisponibles";
        const std::string UBICACION_SERVER = "UbicacionServer";
    }
    
    // Constantes del sistema
    const int MAX_TESTERS_ESPECIALES_PARA_ASIGNAR = 4;

    namespace Mtypes{
        // mtypes desde el dispositivo
        const int MTYPE_REQUERIMIENTO_DISPOSITIVO = 1;
        const int MTYPE_RESULTADO_INICIAL = 2;
        const int MTYPE_RESULTADO_ESPECIAL = 3;
        // mtypes desde tester comun
        const int MTYPE_PROGRAMA_INICIAL = 1;
        const int MTYPE_REGISTRAR_TESTER = 2;
        const int MTYPE_REQUERIMIENTO_TESTER_ESPECIAL = 4;
        // mtypes desde tester especial
        const int MTYPE_TAREA_ESPECIAL = 1;
        const int MTYPE_AVISAR_DISPONIBILIDAD = 3;
        // mtypes desde equipo especial
        const int MTYPE_ORDEN = 1;
        const int MTYPE_FIN_TEST_ESPECIAL = 5;
        const int MTYPE_HAY_QUE_REINICIAR = 6;
        //Offsets
        const int INIT_MTYPE_SHMEM_PLANILLA_GENERAL = ID_EQUIPO_ESPECIAL + 1;
        //Shmem planilla asignacion
        const int MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION = 1;
        const int MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION = 2;
        //Shmem planilla general
        const int MTYPE_REQ_SHMEM_PLANILLA_GENERAL = 3;
        const int MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL = 4;
    }

    //Asociadas a mensajes
    const int RESULTADO_GRAVE = 0;
    const int RESULTADO_NO_GRAVE = 1;
    const int SEGUIR_TESTEANDO = 2;
    const int NO_CONTESTAR = 3;
    const int FIN_TEST_ESPECIAL = 4;
    const int ORDEN_APAGADO = 0;
    const int ORDEN_REINICIO = 1;
    const int ORDEN_SEGUIR_TESTEANDO = 2;
    const int SIN_LUGAR = -1;
}

const int CANT_TESTERS_COMUNES = 5;
const int MAX_TAREAS_ESPECIALES = 10;



// IDs de los IPC
const int SEM_ESPECIALES_ASIGNACION = 14;
const int SHM_BROKER_TESTERS_REGISTRADOS = 15;
const int SEM_BROKER_TESTERS_REGISTRADOS = 16;

const int MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS = 22;
const int MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS = 23;
const int MSGQUEUE_BROKER_RECEPTOR = 24;
const int MSGQUEUE_BROKER_EMISOR = 25;
const int MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER = 42;
const int MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER = 43;
const int MSGQUEUE_BROKER_REQUERIMIENTO_SHMEM_HANDLER = 44;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS = 45;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL = 46;
const int MSGQUEUE_BROKER_REGISTRO_TESTERS = 47;

const int LAST_ID_IPC = MSGQUEUE_BROKER_REGISTRO_TESTERS + 1;
const int SEM_ESPECIALES = LAST_ID_IPC; // Semaforos para testers especiales (creciente)



const int TESTER_ESPECIAL_NO_ASIGNADO = 0;
//const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;
// Archivos necesarios

const std::string logFileName = "log.txt";

// Para los sockets
const char UBICACION_SERVER_IDENTIFICADOR[] = "192.168.2.3";

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

// Primer mensaje de protocolo que cliente receptor le envia al server emisor
typedef struct TFirstMessage {
    int identificador;
} TFirstMessage;

typedef struct message {
    long mtype;
    long mtypeMensaje;
    int idDispositivo;
    int tester;
    bool esTesterEspecial;
    bool hayQueReiniciar; // Se usa para avisarle al tester especial si debe reiniciar o no
    int value; // Este parametro posee el valor del programa, del resultado y de la orden
    int posicionDispositivo;
    int idTestersEspeciales[Constantes::MAX_TESTERS_ESPECIALES_PARA_ASIGNAR];
    int cantTestersEspecialesAsignados;
} TMessageAtendedor;

typedef struct TTablaBrokerTestersRegistrados {
    bool registrados[MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES];
    bool disponibles[MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES];
    int ultimoTesterElegido;
} TTablaBrokerTestersRegistrados;

/*********CONFIG PEDIDO SHARED MEMORY TESTERS**********/
typedef struct TSharedMemoryPlanillaAsignacion {
    long mtype;
    TContadorTesterEspecial cantTestersEspecialesAsignados[MAX_DISPOSITIVOS_EN_SISTEMA];
    TContadorTareaEspecial cantTareasEspecialesAsignadas[MAX_DISPOSITIVOS_EN_SISTEMA];
} TSharedMemoryPlanillaAsignacion;

typedef struct TSharedMemoryPlanillaGeneral {
    long mtype;
    int cantDispositivosSiendoTesteados;
    bool idsPrivadosDispositivos[MAX_DISPOSITIVOS_EN_SISTEMA];
} TSharedMemoryPlanillaGeneral;

typedef struct TRequerimientoSharedMemory {
    long mtype;
    int idSolicitante;
    int idDevolucion;
} TRequerimientoSharedMemory;
