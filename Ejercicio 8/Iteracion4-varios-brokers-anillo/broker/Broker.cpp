#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include "brokersInfo.h"

void crearIpc() {
    
    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::out);
    ipcFile.close();

	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	msgget(key, IPC_CREAT | 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	msgget(key, IPC_CREAT | 0660);
	
	for(int i = MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS; i <= MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER; i++) {
        key = ftok(ipcFileName.c_str(), i);
        msgget(key, IPC_CREAT | 0660);
    }    
    
    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shmCantReqBrokerShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* cantReqBrokerShm = (int*) shmat(shmCantReqBrokerShMem, NULL, 0);
    *cantReqBrokerShm = 0;

    Semaphore semReqBrokerShm(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semReqBrokerShm.creaSem();
    semReqBrokerShm.iniSem(1);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    int shmCantReqPlanillasShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    TShmemCantRequerimientos* cantReqPlanillasShm = (TShmemCantRequerimientos*) shmat(shmCantReqPlanillasShMem, NULL, 0);
    cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion = 0;
    cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral = 0;

    Semaphore semReqPlanillasShm(SEM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    semReqPlanillasShm.creaSem();
    semReqPlanillasShm.iniSem(1);
    
    /* INFO proveniente del algoritmo del anillo*/
    key = ftok(ipcFileName.c_str(), SHM_BROKER_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);
    *soyLider = false;

    Semaphore semSoyLider(SEM_BROKER_ES_LIDER);
    semSoyLider.creaSem();
    semSoyLider.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_BROKER_SIGUIENTE);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguiente = (int*) shmat(shmIdBrokerSiguiente, NULL, 0);
    *idBrokerSiguiente = 0;

    Semaphore semIdBrokerSiguiente(SEM_BROKER_SIGUIENTE);
    semIdBrokerSiguiente.creaSem();
    semIdBrokerSiguiente.iniSem(1);
    
}

void crearModulosBroker() {

	Logger::notice("Creo el modulo de broker pasamanos emisor", __FILE__);
	if (fork() == 0){
		execlp("./brokerPasamanosEmisor", "brokerPasamanosEmisor", (char*)0);
		Logger::notice ("Mensaje luego de execlp de brokerPasamanosEmisor. Algo salio mal!", __FILE__);
        exit(1);
	}

    Logger::notice("Creo el modulo de broker pasamanos receptor", __FILE__);
	if (fork() == 0){
		execlp("./brokerPasamanosReceptor", "brokerPasamanosReceptor", (char*)0);
		Logger::notice ("Mensaje luego de execlp de brokerPasamanosReceptor. Algo salio mal!", __FILE__);
        exit(1);
	}

	Logger::notice("Creo el modulo de broker de nuevos requerimientos de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./brokerRequerimientosDisp", "brokerRequerimientosDisp", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerRequerimientosDisp. Algo salio mal!", __FILE__);
        exit(1);
	}

	Logger::notice("Creo el modulo de broker de requerimientos para testers especiales", __FILE__);
	if (fork() == 0){
		execlp("./brokerReqTestEsp", "brokerReqTestEsp", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerReqTestEsp. Algo salio mal!", __FILE__);
        exit(1);
	}
    
    Logger::notice("Creo el modulo de broker de registro de testers", __FILE__);
    if (fork() == 0){
		execlp("./brokerRegistroTesters", "brokerRegistroTesters", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerRegistroTesters. Algo salio mal!", __FILE__);
        exit(1);
	}
    
    Logger::notice("Creo el modulo de broker paso de mensajes inter-broker", __FILE__);
    if (fork() == 0){
		execlp("./interBrokerMsgHandler", "interBrokerMsgHandler", (char*)0);
        Logger::notice ("Mensaje luego de execlp de interBrokerMsgHandler. Algo salio mal!", __FILE__);
        exit(1);
	}
    
    Logger::notice("Creo el modulo de broker paso de shmem inter-broker", __FILE__);
    if (fork() == 0){
		execlp("./brokerShMemPassingHandler", "brokerShMemPassingHandler", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerShMemPassingHandler. Algo salio mal!", __FILE__);
        exit(1);
	}
    
    Logger::notice("Creo el modulo de broker paso de shmem de tester (planillas) inter-broker", __FILE__);
    if (fork() == 0){
		execlp("./brokerShMemTesterPassingHandler", "brokerShMemTesterPassingHandler", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerShMemTesterPassingHandler. Algo salio mal!", __FILE__);
        exit(1);
	}

    Logger::notice("Creo el modulo de broker contabilizador de requerimientos de planillas", __FILE__);
    if (fork() == 0){
		execlp("./brokerShMemTesterReqHandler", "brokerShMemTesterReqHandler", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerShMemTesterReqHandler. Algo salio mal!", __FILE__);
        exit(1);
	}
}

void crearServers(){
	
    char paramMsgQueue[10];
    char paramIdBroker[10];
    char paramSizeMsg[10];
    
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));
    // Comunicacion con testers y equipo especial
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPTOR);
    Logger::notice("Creo el servidor receptor de mensajes de testers comunes", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR , paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el servidor emisor de mensajes a testers comunes", __FILE__);
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_EMISOR);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR , paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}

	// Comunicacion con dispositivos
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}
	
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS , paramMsgQueue, paramSizeMsg,(char*) 0);
        exit(1);
	}
    
    // Comunicacion Servidor con brokers para mensajes generales
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_DESDE_BROKER);
    Logger::notice("Creo el servidor receptor de mensajes generales de brokers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_CONTRA_BROKERS , paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
	}
    
    // Comunicacion Servidor con brokers para memoria compartida de los brokers
    sprintf(paramMsgQueue, "%d", MSGQUEUE_RECEPCION_BROKER_SHM);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageShMemInterBroker));
    Logger::notice("Creo el servidor receptor de memoria compartida de brokers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_CONTRA_BROKERS_SHMEM_BROKERS , paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
	}
    
    // Comunicacion Servidor con brokers para memoria compartida de planilla general
    sprintf(paramMsgQueue, "%d", MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));
    Logger::notice("Creo el servidor receptor de memoria compartida de brokers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor",  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_GENERAL, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
	}
    
    // Comunicacion Servidor con brokers para memoria compartida de planilla asignacion
    sprintf(paramMsgQueue, "%d", MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
    Logger::notice("Creo el servidor receptor de memoria compartida de brokers", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor",  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_ASIGNACION, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
	}
    
    // Comunicacion con tester/eq-esp para la shared memory de planilla general
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL, paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}

	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_ENVIO_SHM_PLANILLA_GENERAL, paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}

    // Comunicacion con tester/eq-esp para la shared memory de planilla asignacion
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPCION_SHM_PLANILLA_ASIGNACION, paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}

	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_ENVIO_SHM_PLANILLA_ASIGNACION, paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}
    
    // Comunicacion con tester/eq-esp para obtener requerimientos de planillas
    sprintf(paramSizeMsg, "%d", (int) sizeof(TRequerimientoSharedMemory));
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_REQUERIMIENTO_SHMEM_HANDLER);
    Logger::notice("Creo el servidor receptor de mensajes de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPCION_REQ_PLANILLAS , paramMsgQueue, paramSizeMsg, (char*)0);
        exit(1);
	}

    // Para dar tiempo a que todos los broker esten arriba
	sleep(10);
    // Obtengo el ID del siguiente broker
    key_t key = ftok(ipcFileName.c_str(), SHM_BROKER_SIGUIENTE);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguiente = (int*) shmat(shmIdBrokerSiguiente, NULL, 0);
    Semaphore semIdBrokerSiguiente(SEM_BROKER_SIGUIENTE);
    semIdBrokerSiguiente.getSem();
    semIdBrokerSiguiente.p();
    int brokerSiguiente = *idBrokerSiguiente;
    semIdBrokerSiguiente.v();
    // Comunicacion cliente con brokers para mensajes generales
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_HACIA_BROKER);
    for (int i = 0; i < CANT_BROKERS; i++) {
        if (i == (ID_BROKER - ID_BROKER_START)) continue;
        sprintf(paramIdBroker, "%d", i + ID_BROKER_START);
        Logger::notice("Creo el cliente emisor de mensajes generales a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i].ipBroker, PUERTO_CONTRA_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
    }
    
    // Comunicacion cliente con brokers para memoria compartida de los brokers
    sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageShMemInterBroker));
    sprintf(paramIdBroker, "%d", 0); // La conexion es solo con el sgte broker por lo que agarra todo lo que esta en la MsgQueue
    Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
    if (fork() == 0){
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[brokerSiguiente - ID_BROKER_START].ipBroker, PUERTO_CONTRA_BROKERS_SHMEM_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
    }

    // Comunicacion cliente con brokers para memoria compartida de planilla general
    sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_GENERAL);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));
    sprintf(paramIdBroker, "%d", 0); // La conexion es solo con el sgte broker por lo que agarra todo lo que esta en la MsgQueue
    Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
    if (fork() == 0){
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[brokerSiguiente - ID_BROKER_START].ipBroker,  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_GENERAL, paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
    }
    
    // Comunicacion cliente con brokers para memoria compartida de planilla asignacion
    sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_ASIGNACION);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
    sprintf(paramIdBroker, "%d", 0); // La conexion es solo con el sgte broker por lo que agarra todo lo que esta en la MsgQueue
    Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
    if (fork() == 0){
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[brokerSiguiente - ID_BROKER_START].ipBroker,  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_ASIGNACION, paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
    }
    
}

void lanzarTareasMaster() {
    
    key_t key = ftok(ipcFileName.c_str(), SHM_BROKER_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);
    *soyLider = false;

    Semaphore semSoyLider(SEM_BROKER_ES_LIDER);
    semSoyLider.getSem();
    
    semSoyLider.p();
    bool lanzarTareas = *soyLider;
    semSoyLider.v();

    if(lanzarTareas) {
        // Tengo que inicializar y lanzar la memoria compartida entre brokers
        TMessageShMemInterBroker msgShmemInterBroker;
        msgShmemInterBroker.mtype = ID_BROKER;

        for (int i = 0; i < MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES + 1; i++) {
            msgShmemInterBroker.memoria.registrados[i] = false;
            msgShmemInterBroker.memoria.brokerAsignado[i] = TESTER_ESPECIAL_NO_ASIGNADO;
            msgShmemInterBroker.memoria.disponible[i] = false;
        }
        msgShmemInterBroker.memoria.ultimoTesterElegido = 0;

        Logger::notice("Soy el MASTER y me lanzo la memoria compartida para iniciar el flujo", __FILE__);
        key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM);
        int msgQueueRecepcionShmem = msgget(key, IPC_CREAT | 0660);
        int okSend = msgsnd(msgQueueRecepcionShmem, &msgShmemInterBroker, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al iniciar el envio de la Shmem a mi mismo (MASTER). Abort!", __FILE__);
            exit(1);
        }

        // Inicializo y lanzo la planilla general
        TSharedMemoryPlanillaGeneral planillaGeneral;
        planillaGeneral.mtype = ID_BROKER;
        planillaGeneral.cantDispositivosSiendoTesteados = 0;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++)    planillaGeneral.idsPrivadosDispositivos[i] = false;

        key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL);
        int msgQueueRecepcionPlanillaGral = msgget(key, IPC_CREAT | 0660);
        okSend = msgsnd(msgQueueRecepcionPlanillaGral, &planillaGeneral, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al iniciar el envio de la planilla general a mi mismo (MASTER). Abort!", __FILE__);
            exit(1);
        }

        // Inicializo y lanzo la planilla asignacion
        TSharedMemoryPlanillaAsignacion planillaAsignacion;
        planillaAsignacion.mtype = ID_BROKER;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++) {
            planillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTotal = 0;
            planillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTerminadas = 0;
            planillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTotal = 0;
            planillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTerminados = 0;
        }

        key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION);
        int msgQueueRecepcionPlanillaAsig = msgget(key, IPC_CREAT | 0660);
        okSend = msgsnd(msgQueueRecepcionPlanillaAsig, &planillaAsignacion, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al iniciar el envio de la planilla asignacion a mi mismo (MASTER). Abort!", __FILE__);
            exit(1);
        }
    }
}

void armarAnillo() {
        /* LANZO EL PROGRAMA CORRESPONDIENTE PARA LA GENERACION DEL ANILLO*/
    Logger::notice("Lanzo programa correspondiente para generar el anillo!", __FILE__);
    std::stringstream ringProgram, program;
    ringProgram << "./anillo/";
    if (ID_BROKER == MASTER_BROKER) {
        program << "sender";
    } else {
        program << "listener";
    }
    ringProgram << program.str();
    char paramId[10];
    sprintf(paramId, "%d", ID_BROKER);
    if(fork() == 0) {
        execlp(ringProgram.str().c_str(), program.str().c_str(), paramId, (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del programa del anillo. Algo salio mal!", __FILE__);
        exit(1);
    }
    wait(NULL);
    /*ANILLO SUPUESTAMENTE GENERADO*/
    Logger::notice("Programa del anillo termina. Supuestamente generado!", __FILE__);
}

int main (int argc, char* argv[]) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    crearIpc();
    armarAnillo();
	crearServers();
	crearModulosBroker();
    
    return 0;
}
