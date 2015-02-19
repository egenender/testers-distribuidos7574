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

#include <string>
#include <stddef.h>
#include <stdint.h>

// Constantes del sistema

const int CANT_TESTERS_COMUNES = 5;
const int MAX_TESTER_COMUNES = 30;
const int CANT_TESTERS_ESPECIALES = 30;
const int MAX_TESTER_ESPECIALES = 60;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;
const int MAX_TAREAS_ESPECIALES = 10;

static int ID_DISPOSITIVO = 5;
const int ID_TESTER_COMUN_START = ID_DISPOSITIVO + MAX_DISPOSITIVOS_EN_SISTEMA;
const int ID_TESTER_ESP_START = ID_TESTER_COMUN_START + MAX_TESTER_COMUNES;
const int ID_EQUIPO_ESPECIAL = ID_TESTER_ESP_START + MAX_TESTER_ESPECIALES;

//////////////// IPCS ///////////////////

///////////// IPC ANILLO ///////////////
const int SHM_BROKER_ES_LIDER = 1;
const int SEM_BROKER_ES_LIDER = 2;
const int SHM_BROKER_SIGUIENTE = 3;
const int SEM_BROKER_SIGUIENTE = 4;
const int SHM_BROKER_VERSION = 5;
const int SEM_BROKER_VERSION = 6;
const int SHM_PLANILLA_GENERAL_ES_LIDER = 7;
const int SEM_PLANILLA_GENERAL_ES_LIDER = 8;
const int SHM_PLANILLA_GENERAL_SIGUIENTE = 9;
const int SEM_PLANILLA_GENERAL_SIGUIENTE = 10;
const int SHM_PLANILLA_GENERAL_VERSION = 11;
const int SEM_PLANILLA_GENERAL_VERSION = 12;
const int SHM_PLANILLA_ASIGNACION_ES_LIDER = 13;
const int SEM_PLANILLA_ASIGNACION_ES_LIDER = 14;
const int SHM_PLANILLA_ASIGNACION_SIGUIENTE = 15;
const int SEM_PLANILLA_ASIGNACION_SIGUIENTE = 16;
const int SHM_PLANILLA_ASIGNACION_VERSION = 17;
const int SEM_PLANILLA_ASIGNACION_VERSION = 18;
const int SEM_ANILLO_BROKER_SHM_RESTAURANDOSE = 19;
const int SHM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE = 20;
const int SEM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE = 21;
const int SEM_ANILLO_PLANILLA_GENERAL_RESTAURANDOSE = 22;
const int SHM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE = 23;
const int SEM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE = 24;
const int SEM_ANILLO_PLANILLA_ASIGNACION_RESTAURANDOSE = 25;
const int SHM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE = 26;
const int SEM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE = 27;
const int SHM_LISTENER_BROKER_SHM_PID = 28;
const int SHM_LISTENER_PLANILLA_GENERAL_PID = 29;
const int SHM_LISTENER_PLANILLA_ASIGNACION_PID = 30;

const int SHM_TESTERS_COMUNES_DISPONIBLES = 31;
const int SHM_TESTERS_ESPECIALES_DISPONIBLES = 32;

const int SEM_TABLA_TESTERS_COMUNES_DISPONIBLES = 33;
const int SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES = 34;

const int SEM_IDENTIFICADOR = 35;

const int SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM = 36;
const int SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM = 37;

const int SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM = 38;
const int SEM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM = 39;

const int MSGQUEUE_ENVIO_DISP = 120;
const int MSGQUEUE_RECEPCIONES_DISP = 121;
const int MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS = 122;
const int MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS = 123;
const int MSGQUEUE_BROKER_RECEPTOR = 124;
const int MSGQUEUE_BROKER_EMISOR = 125;
const int MSGQUEUE_ENVIO_TESTER_COMUN = 126;
const int MSGQUEUE_RECEPCIONES_TESTER_COMUN = 127;
const int MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN = 128;
const int MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN = 129;
const int MSGQUEUE_ENVIO_TESTER_ESPECIAL = 130;
const int MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL = 131;
const int MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL = 132;
const int MSGQUEUE_ENVIO_EQUIPO_ESPECIAL = 133;
const int MSGQUEUE_DESPACHADOR = 134;
const int MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL = 135;
const int MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL = 136;
const int MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION = 137;
const int MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION = 138;
const int MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS = 139;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS = 140;
const int MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL = 141;
const int MSGQUEUE_BROKER_REGISTRO_TESTERS = 142;

/* NUEVOS IPCS PARA MSG-INTER-BROKERS*/
const int MSGQUEUE_BROKER_HACIA_BROKER = 143;
const int MSGQUEUE_BROKER_DESDE_BROKER = 144;

/* MSGQUEUE PARA LA SHMEM INTER-BROKER (TESTERS DISPONIBLES)*/
const int MSGQUEUE_INTERNAL_BROKER_SHM = 145;
const int MSGQUEUE_ENVIO_BROKER_SHM = 146; // Envio de la shm inter-broker a otros brokers
const int MSGQUEUE_RECEPCION_BROKER_SHM = 147; // Recepcion de la shm inter-broker desde otros brokers

/* MSGQUEUE PARA LAS SHMEMS PLANILLAS ENTRE-BROKERS*/
const int MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL = 148;
const int MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION = 149;
const int MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_GENERAL = 150;
const int MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_ASIGNACION = 151;

/* MSGQUEUE PARA LAS SHMEMS PLANILLAS HACIA TESTER/EQUIPO*/
const int MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER = 152;
const int MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER = 153;
const int MSGQUEUE_BROKER_REQUERIMIENTO_SHMEM_HANDLER = 154;

/* MSGQUEUE PARA USO INTERNO DEL BROKER CONTADOR DE REQUERIMIENTOS DE SHMEM DE PLANILLAS*/
const int MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER = 155;

const int LAST_ID_IPC = MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER + 1;

const int SEM_ESPECIALES = LAST_ID_IPC + 1;

// mtypes desde el dispositivo
const int MTYPE_REQUERIMIENTO_DISPOSITIVO = 1;
const int MTYPE_RESULTADO_INICIAL = 2;
const int MTYPE_RESULTADO_ESPECIAL = 3;
// mtypes desde tester comun
const int MTYPE_PROGRAMA_INICIAL = 1;
const int MTYPE_REGISTRAR_TESTER = 2;
const int MTYPE_DESREGISTRAR_TESTER = 7;
const int MTYPE_REQUERIMIENTO_TESTER_ESPECIAL = 4;
// mtypes desde tester especial
const int MTYPE_TAREA_ESPECIAL = 1;
const int MTYPE_AVISAR_DISPONIBILIDAD = 6;
// mtypes desde equipo especial
const int MTYPE_ORDEN = 1;
const int MTYPE_HAY_QUE_REINICIAR = 3;
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

const int MAX_TESTERS_ESPECIALES_PARA_ASIGNAR = 4;
const int TESTER_ESPECIAL_NO_ASIGNADO = 0;
//const int MINIMO_TESTERS_ESPECIALES_POR_ESPECIFICACION = 2;
// Archivos necesarios

const std::string ipcFileName = "/tmp/pereira-ipcs";

const std::string logFileName = "log.txt";

const std::string configBrokerShmemFileName = "/home/ferno/ferno/FIUBA/Distribuidos/Practicas/testers-distribuidos7574/Ejercicio 8/Iteracion4-varios-brokers-anillo/Anillo/configBrokerShm"; // Relativo a ./bin/anillo/

const std::string configPlanillaGeneralShmemFileName = "/home/ferno/ferno/FIUBA/Distribuidos/Practicas/testers-distribuidos7574/Ejercicio 8/Iteracion4-varios-brokers-anillo/Anillo/configPlanillaGeneralShmem"; // Relativo a ./bin/anillo/

const std::string configPlanillaAsignacionShmemFileName = "/home/ferno/ferno/FIUBA/Distribuidos/Practicas/testers-distribuidos7574/Ejercicio 8/Iteracion4-varios-brokers-anillo/Anillo/configPlanillaAsignacionShmem"; // Relativo a ./bin/anillo/

//Estructuras communes:
typedef struct resultado{
	int32_t idDispositivo;
	int32_t resultadosPendientes;
	int32_t resultadosGraves;
}resultado_t;

typedef struct TContadorTesterEspecial {
    int32_t cantTestersEspecialesTotal;
    int32_t cantTestersEspecialesTerminados;
} TContadorTesterEspecial;

typedef struct TContadorTareaEspecial {
    int32_t cantTareasEspecialesTotal;
    int32_t cantTareasEspecialesTerminadas;
} TContadorTareaEspecial;

typedef struct TMessageReinicioTest {
    int64_t mtype;
    int8_t hayQueReiniciar;
} TMessageReinicioTest;

// Primer mensaje de protocolo que cliente receptor le envia al server emisor
typedef struct TFirstMessage {
    int32_t identificador;
} TFirstMessage;

typedef struct message {
    int64_t mtype;
    int64_t mtypeMensaje;
    int64_t mtypeMensajeBroker;
    int32_t idDispositivo;
    int32_t tester;
    int8_t esTesterEspecial;
    int8_t hayQueReiniciar; // Se usa para avisarle al tester especial si debe reiniciar o no
    int32_t idBroker;
    int32_t idBrokerOrigen;
    int32_t value; // Este parametro posee el valor del programa, del resultado y de la orden
    int32_t posicionDispositivo;
    int32_t idTestersEspeciales[MAX_TESTERS_ESPECIALES_PARA_ASIGNAR];
    int32_t cantTestersEspecialesAsignados;
} TMessageAtendedor;

typedef struct TTablaBrokerTestersRegistrados {
    int8_t registrados[MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES + 1];
    int8_t disponible[MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES + 1];
    int32_t brokerAsignado[MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES + 1]; // Especifica el ID del broker de cada ID
    int32_t ultimoTesterElegido;
} TTablaBrokerTestersRegistrados;

// Para el server RPC de identificadores
typedef struct TTablaIdTestersDisponibles {
    int8_t disponibles[MAX_TESTER_COMUNES];
} TTablaIdTestersDisponibles;

typedef struct TTablaIdTestersEspecialesDisponibles {
    int8_t disponibles[MAX_TESTER_ESPECIALES];
} TTablaIdTestersEspecialesDisponibles;

/*******BROKERS CONFIG!********/
const int MASTER_BROKER = 1;
const int ID_BROKER_START = 1;
const int CANT_BROKERS = 3;
const int ID_BROKER = 1;
const int ID_BROKER_SIGUIENTE = (ID_BROKER == CANT_BROKERS) ? ID_BROKER_START : ID_BROKER + 1;
// LAS IPS DE LOS BROKERS ESTA EN EL ARCHIVO BROKER.CPP
const char PUERTO_CONTRA_BROKERS[] = "40000";
const char PUERTO_CONTRA_BROKERS_SHMEM_BROKERS[] = "40005";
const char PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_GENERAL[] = "40010";
const char PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_ASIGNACION[] = "40015";

// Para el mtypeMessageBroker del InterBroker-Message-Handler
const int MTYPE_HACIA_DISPOSITIVO = 1;
const int MTYPE_HACIA_TESTER = 2;
const int MTYPE_HACIA_EQUIPO_ESPECIAL = 3;

// Para los sockets
const char PUERTO_SERVER_RECEPTOR_DISPOSITIVOS[] = "50000";
const char PUERTO_SERVER_EMISOR_DISPOSITIVOS[] = "50001";

const char PUERTO_SERVER_RECEPTOR[] = "50002";
const char PUERTO_SERVER_EMISOR[] = "50003";

// Para el broker al que se conectan dispositivos y testers de esta maquina
const char UBICACION_SERVER[] = "127.0.0.1";
//const char UBICACION_SERVER_IDENTIFICADOR[] = "192.168.2.7";
const char UBICACION_SERVER_IDENTIFICADOR[] = "192.168.2.6";

/*********CONFIG PEDIDO SHARED MEMORY TESTERS**********/
typedef struct TSharedMemoryPlanillaAsignacion {
    int64_t mtype;
    uint64_t version;
    TContadorTesterEspecial cantTestersEspecialesAsignados[MAX_DISPOSITIVOS_EN_SISTEMA];
    TContadorTareaEspecial cantTareasEspecialesAsignadas[MAX_DISPOSITIVOS_EN_SISTEMA];
} TSharedMemoryPlanillaAsignacion;

const int MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION = 1;
const int MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION = 2;
const char PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION[] = "60011";
const char PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION[] = "60012";

typedef struct TSharedMemoryPlanillaGeneral {
    int64_t mtype;
    uint64_t version;
    int32_t cantDispositivosSiendoTesteados;
    bool idsPrivadosDispositivos[MAX_DISPOSITIVOS_EN_SISTEMA];
} TSharedMemoryPlanillaGeneral;

const int MTYPE_REQ_SHMEM_PLANILLA_GENERAL = 3;
const int MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL = 4;
const int INIT_MTYPE_SHMEM_PLANILLA_GENERAL = ID_EQUIPO_ESPECIAL + 1;
const char PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL[] = "60013";
const char PUERTO_SERVER_ENVIO_SHM_PLANILLA_GENERAL[] = "60014";

typedef struct TRequerimientoSharedMemory {
    int64_t mtype;
    int32_t idSolicitante;
    int32_t idDevolucion;
} TRequerimientoSharedMemory;
const char PUERTO_SERVER_RECEPCION_REQ_PLANILLAS[] = "60015";

typedef struct TShmemCantRequerimientos {
    int32_t cantRequerimientosShmemPlanillaGeneral;
    int32_t cantRequerimientosShmemPlanillaAsignacion;
} TShmemCantRequerimientos;
/*******FIN PEDIDO SHARED MEMORY TESTERS***********/

/*******SUB-BROKERS-CONFIG********/
const int MTYPE_DEVOLUCION_SHM_BROKER = 1;
const int MTYPE_REQUERIMIENTO_SHM_BROKER = 2; // Procurar que el ID de los subbrokers superen esto
const int ID_SUB_BROKER_REQUERIMIENTO_DISP = 5;
const int ID_SUB_BROKER_REQUERIMIENTO_ESP = 6;
const int ID_SUB_BROKER_REGISTRO_TESTER = 7;
const int ID_SUB_BROKER_PASAMANOS_RECEPTOR = 8;

typedef struct TMessageShMemInterBroker {
    int64_t mtype;
    uint64_t version;
    TTablaBrokerTestersRegistrados memoria;
} TMessageShMemInterBroker;

typedef struct TMessageRequerimientoBrokerShm {
    int64_t mtype;
    int32_t idSubBroker;
} TMessageRequerimientoBrokerShm;
/*******FIN SUB-BROKERS CONFIG*********/

/******** CONFIG PARA TIMEOUT *********/
const int SLEEP_TIMEOUT_SHMEM = 15; // Timeout para la obtencion de cualquier shmem
const int SLEEP_TIMEOUT_DISPOSITIVO = 60;
const int SLEEP_TIMEOUT_TESTERS = 60;

#endif	/* COMMON_H */

