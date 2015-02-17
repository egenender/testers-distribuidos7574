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
    shmdt((void*) cantReqBrokerShm);

    Semaphore semReqBrokerShm(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semReqBrokerShm.creaSem();
    semReqBrokerShm.iniSem(1);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    int shmCantReqPlanillasShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    TShmemCantRequerimientos* cantReqPlanillasShm = (TShmemCantRequerimientos*) shmat(shmCantReqPlanillasShMem, NULL, 0);
    cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion = 0;
    cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral = 0;
    shmdt((void*) cantReqPlanillasShm);

    Semaphore semReqPlanillasShm(SEM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    semReqPlanillasShm.creaSem();
    semReqPlanillasShm.iniSem(1);
    
    /* INFO proveniente del algoritmo del anillo para la shmem inter-broker*/
    key = ftok(ipcFileName.c_str(), SHM_BROKER_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);
    *soyLider = false;
    shmdt((void*) soyLider);

    Semaphore semSoyLider(SEM_BROKER_ES_LIDER);
    semSoyLider.creaSem();
    semSoyLider.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_BROKER_SIGUIENTE);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguiente = (int*) shmat(shmIdBrokerSiguiente, NULL, 0);
    *idBrokerSiguiente = 0;
    shmdt((void*) idBrokerSiguiente);

    Semaphore semIdBrokerSiguiente(SEM_BROKER_SIGUIENTE);
    semIdBrokerSiguiente.creaSem();
    semIdBrokerSiguiente.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_BROKER_VERSION);
    int shmIdBrokerVersion = shmget(key, sizeof(unsigned long), IPC_CREAT | 0660);
    unsigned long* idBrokerVersion = (unsigned long*) shmat(shmIdBrokerVersion, NULL, 0);
    *idBrokerVersion = ID_BROKER;
    shmdt((void*) idBrokerVersion);

    Semaphore semIdBrokerVersion(SEM_BROKER_VERSION);
    semIdBrokerVersion.creaSem();
    semIdBrokerVersion.iniSem(1);
    
    /* INFO proveniente del algoritmo del anillo para la shmem planilla general*/
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_ES_LIDER);
    int shmPlanillaGeneralIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLiderPlanillaGeneral = (bool*) shmat(shmPlanillaGeneralIdSoyLider, NULL, 0);
    *soyLiderPlanillaGeneral = false;
    shmdt((void*) soyLiderPlanillaGeneral);

    Semaphore semSoyLiderPlanillaGeneral(SEM_PLANILLA_GENERAL_ES_LIDER);
    semSoyLiderPlanillaGeneral.creaSem();
    semSoyLiderPlanillaGeneral.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_SIGUIENTE);
    int shmIdBrokerSiguientePlanillaGeneral = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguientePlanillaGeneral = (int*) shmat(shmIdBrokerSiguientePlanillaGeneral, NULL, 0);
    *idBrokerSiguientePlanillaGeneral = 0;
    shmdt((void*) idBrokerSiguientePlanillaGeneral);

    Semaphore semIdBrokerSiguientePlanillaGeneral(SEM_PLANILLA_GENERAL_SIGUIENTE);
    semIdBrokerSiguientePlanillaGeneral.creaSem();
    semIdBrokerSiguientePlanillaGeneral.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_VERSION);
    int shmPlanillaGeneralVersionId = shmget(key, sizeof(unsigned long), IPC_CREAT | 0660);
    unsigned long* planillaGeneralVersion = (unsigned long*) shmat(shmPlanillaGeneralVersionId, NULL, 0);
    *planillaGeneralVersion = ID_BROKER;
    shmdt((void*) planillaGeneralVersion);

    Semaphore semPlanillaGeneralVersion(SEM_PLANILLA_GENERAL_VERSION);
    semPlanillaGeneralVersion.creaSem();
    semPlanillaGeneralVersion.iniSem(1);

    /* INFO proveniente del algoritmo del anillo para la shmem planilla asignacion*/
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_ES_LIDER);
    int shmPlanillaAsignacionIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLiderPlanillaAsignacion = (bool*) shmat(shmPlanillaAsignacionIdSoyLider, NULL, 0);
    *soyLiderPlanillaAsignacion = false;
    shmdt((void*) soyLiderPlanillaAsignacion);

    Semaphore semSoyLiderPlanillaAsignacion(SEM_PLANILLA_ASIGNACION_ES_LIDER);
    semSoyLiderPlanillaAsignacion.creaSem();
    semSoyLiderPlanillaAsignacion.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_SIGUIENTE);
    int shmIdBrokerSiguientePlanillaAsignacion = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguientePlanillaAsignacion = (int*) shmat(shmIdBrokerSiguientePlanillaAsignacion, NULL, 0);
    *idBrokerSiguientePlanillaAsignacion = 0;
    shmdt((void*) idBrokerSiguientePlanillaAsignacion);

    Semaphore semIdBrokerSiguientePlanillaAsignacion(SEM_PLANILLA_ASIGNACION_SIGUIENTE);
    semIdBrokerSiguientePlanillaAsignacion.creaSem();
    semIdBrokerSiguientePlanillaAsignacion.iniSem(1);
    
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_VERSION);
    int shmPlanillaAsignacionVersionId = shmget(key, sizeof(unsigned long), IPC_CREAT | 0660);
    unsigned long* planillaAsignacionVersion = (unsigned long*) shmat(shmPlanillaAsignacionVersionId, NULL, 0);
    *planillaAsignacionVersion = ID_BROKER;
    shmdt((void*) planillaAsignacionVersion);

    Semaphore semPlanillaAsignacionVersion(SEM_PLANILLA_ASIGNACION_VERSION);
    semPlanillaAsignacionVersion.creaSem();
    semPlanillaAsignacionVersion.iniSem(1);
    
    /* IPCs funcionales para checkear ejecucion de listener/sender */
    Semaphore semAnilloRestaurandose(SEM_ANILLO_BROKER_SHM_RESTAURANDOSE);
    semAnilloRestaurandose.creaSem();
    semAnilloRestaurandose.iniSem(0);
    
    key = ftok(ipcFileName.c_str(), SHM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE);
    int shmListenerEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerEjecutandose = (bool*) shmat(shmListenerEjecutandose, NULL, 0);
    *listenerEjecutandose = false;
    shmdt((void*) listenerEjecutandose);
    
    Semaphore semListenerEjecutandose(SEM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE);
    semListenerEjecutandose.creaSem();
    semListenerEjecutandose.iniSem(1);
    
    Semaphore semAnilloPlanillaGeneralRestaurandose(SEM_ANILLO_PLANILLA_GENERAL_RESTAURANDOSE);
    semAnilloPlanillaGeneralRestaurandose.creaSem();
    semAnilloPlanillaGeneralRestaurandose.iniSem(0);
    
    key = ftok(ipcFileName.c_str(), SHM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE);
    int shmListenerPlanillaGeneralEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerPlanillaGeneralEjecutandose = (bool*) shmat(shmListenerPlanillaGeneralEjecutandose, NULL, 0);
    *listenerPlanillaGeneralEjecutandose = false;
    shmdt((void*) listenerPlanillaGeneralEjecutandose);
    
    Semaphore semListenerPlanillaGeneralEjecutandose(SEM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE);
    semListenerPlanillaGeneralEjecutandose.creaSem();
    semListenerPlanillaGeneralEjecutandose.iniSem(1);
    
    Semaphore semAnilloPlanillaAsignacionRestaurandose(SEM_ANILLO_PLANILLA_ASIGNACION_RESTAURANDOSE);
    semAnilloPlanillaAsignacionRestaurandose.creaSem();
    semAnilloPlanillaAsignacionRestaurandose.iniSem(0);
    
    key = ftok(ipcFileName.c_str(), SHM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE);
    int shmListenerPlanillaAsignacionEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerPlanillaAsignacionEjecutandose = (bool*) shmat(shmListenerPlanillaAsignacionEjecutandose, NULL, 0);
    *listenerPlanillaAsignacionEjecutandose = false;
    shmdt((void*) listenerPlanillaAsignacionEjecutandose);
    
    Semaphore semListenerPlanillaAsignacionEjecutandose(SEM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE);
    semListenerPlanillaAsignacionEjecutandose.creaSem();
    semListenerPlanillaAsignacionEjecutandose.iniSem(1);
    
    // En estas memorias compartidas estaran los pid de los listener para killearlos si
    // hay que correr el sender
    key = ftok(ipcFileName.c_str(), SHM_LISTENER_BROKER_SHM_PID);
    int shmListenerBrokerPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
    pid_t* listenerBrokerPid = (pid_t*) shmat(shmListenerBrokerPid, NULL, 0);
    *listenerBrokerPid = 0;
    shmdt((void*) listenerBrokerPid);
    
    key = ftok(ipcFileName.c_str(), SHM_LISTENER_PLANILLA_GENERAL_PID);
    int shmListenerPlanillaGeneralPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
    pid_t* listenerPlanillaGeneralPid = (pid_t*) shmat(shmListenerPlanillaGeneralPid, NULL, 0);
    *listenerPlanillaGeneralPid = 0;
    shmdt((void*) listenerPlanillaGeneralPid);
    
    key = ftok(ipcFileName.c_str(), SHM_LISTENER_PLANILLA_ASIGNACION_PID);
    int shmListenerPlanillaAsignacionPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
    pid_t* listenerPlanillaAsignacionPid = (pid_t*) shmat(shmListenerPlanillaAsignacionPid, NULL, 0);
    *listenerPlanillaAsignacionPid = 0;
    shmdt((void*) listenerPlanillaAsignacionPid);
    
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

    // Creo emisores de mensajes entre brokers (mensajes generales y memoria compartida)
    for (int i = 0; i < CANT_BROKERS; i++) {

        if (i == (ID_BROKER - ID_BROKER_START)) continue; // No conectarse con uno mismo

        // Comunicacion de mensajes generales
        sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));
        sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_HACIA_BROKER);
        sprintf(paramIdBroker, "%d", i + ID_BROKER_START);
        Logger::notice("Creo el cliente emisor de mensajes generales a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i].ipBroker, PUERTO_CONTRA_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
        
        // Comunicacion de memoria compartida inter-brokers
        sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM);
        sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageShMemInterBroker));
        Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i].ipBroker, PUERTO_CONTRA_BROKERS_SHMEM_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
        
        // Comunicacion de memoria compartida de planilla general
        sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_GENERAL);
        sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));
        Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i].ipBroker,  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_GENERAL, paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
        
        // Comunicacion de memoria compartida de planilla asignacion
        sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_ASIGNACION);
        sprintf(paramSizeMsg, "%d", (int) sizeof(TSharedMemoryPlanillaAsignacion));
        Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i].ipBroker,  PUERTO_CONTRA_BROKERS_SHMEM_PLANILLA_ASIGNACION, paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
    }
    
}

// Esta funcion generica verifica en la shmem pasada por parametro si soy lider de algun anillo)
bool soyLider(int shMemId, int semId) {
    key_t key = ftok(ipcFileName.c_str(), shMemId);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);
    *soyLider = false;

    Semaphore semSoyLider(semId);
    semSoyLider.getSem();
    
    semSoyLider.p();
    bool lanzarTareas = *soyLider;
    semSoyLider.v();
    
    shmdt((void*) soyLider);
    
    return lanzarTareas;
}

void lanzarShmem(int msgQueueId, void* msg, size_t sizeMsg) {

    key_t key = ftok(ipcFileName.c_str(), msgQueueId);
    int msgQueueRecepcionShmem = msgget(key, IPC_CREAT | 0660);
    int okSend = msgsnd(msgQueueRecepcionShmem, msg, sizeMsg - sizeof(long), 0);
    if (okSend == -1) {
        Logger::error("Error al iniciar el envio de la Shmem a mi mismo (MASTER). Abort!", __FILE__);
        exit(1);
    }
}

void lanzarTareasMaster() {

    // Si soy lider del anillo logico de la shmem inter-broker, lanzo la shmem
    if(soyLider(SHM_BROKER_ES_LIDER, SEM_BROKER_ES_LIDER)) {
        // Tengo que inicializar y lanzar la memoria compartida entre brokers
        TMessageShMemInterBroker msgShmemInterBroker;
        msgShmemInterBroker.mtype = ID_BROKER;

        for (int i = 0; i < MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES + 1; i++) {
            msgShmemInterBroker.memoria.registrados[i] = false;
            msgShmemInterBroker.memoria.brokerAsignado[i] = TESTER_ESPECIAL_NO_ASIGNADO;
            msgShmemInterBroker.memoria.disponible[i] = false;
        }
        msgShmemInterBroker.memoria.ultimoTesterElegido = 0;

        Logger::notice("Soy el LIDER del anillo logico de la shmem inter-broker y me lanzo la memoria compartida para iniciar el flujo", __FILE__);
        lanzarShmem(MSGQUEUE_RECEPCION_BROKER_SHM, &msgShmemInterBroker, sizeof(TMessageShMemInterBroker));
    }

    // Si soy lider del anillo logico de la shmem de la planilla general, lanzo la shmem
    if(soyLider(SHM_PLANILLA_GENERAL_ES_LIDER, SEM_PLANILLA_GENERAL_ES_LIDER)) {
        // Inicializo y lanzo la planilla general
        TSharedMemoryPlanillaGeneral planillaGeneral;
        planillaGeneral.mtype = ID_BROKER;
        planillaGeneral.cantDispositivosSiendoTesteados = 0;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++)    planillaGeneral.idsPrivadosDispositivos[i] = false;

        Logger::notice("Soy el LIDER del anillo logico de la shmem de la planilla general y me lanzo la memoria compartida para iniciar el flujo", __FILE__);
        lanzarShmem(MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL, &planillaGeneral, sizeof(TSharedMemoryPlanillaGeneral));
    }

    // Si soy lider del anillo logico de la shmem de la planilla asignacion, lanzo la shmem
    if(soyLider(SHM_PLANILLA_ASIGNACION_ES_LIDER, SEM_PLANILLA_ASIGNACION_ES_LIDER)) {
        // Inicializo y lanzo la planilla asignacion
        TSharedMemoryPlanillaAsignacion planillaAsignacion;
        planillaAsignacion.mtype = ID_BROKER;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++) {
            planillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTotal = 0;
            planillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTerminadas = 0;
            planillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTotal = 0;
            planillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTerminados = 0;
        }

        Logger::notice("Soy el LIDER del anillo logico de la shmem de la planilla asignacion y me lanzo la memoria compartida para iniciar el flujo", __FILE__);
        lanzarShmem(MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION, &planillaAsignacion, sizeof(TSharedMemoryPlanillaAsignacion));
    }
}

void armarAnillo() {
    pid_t ringPids[3]; // Uno por cada shared memory
    int i = 0;

    /* LANZO EL PROGRAMA CORRESPONDIENTE PARA LA GENERACION DEL ANILLO*/
    Logger::notice("Lanzo programa correspondiente para generar el anillo!", __FILE__);
    std::stringstream ringProgram, program;
    ringProgram << "./anillo/";
    if (ID_BROKER == MASTER_BROKER) {
        sleep(5); // Espero un poco para que arranquen todos los listener
        program << "sender";
    } else {
        program << "listener";
    }
    ringProgram << program.str();

    char paramFirstTime[10];
    sprintf(paramFirstTime, "%d", 1);
    // Anillo logico de la shared memory inter-broker
    ringPids[i] = fork();
    if(ringPids[i++] == 0) {
        execlp(ringProgram.str().c_str(), program.str().c_str(), configBrokerShmemFileName.c_str(), paramFirstTime, (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del programa del anillo de la shmem inter-broker. Algo salio mal!", __FILE__);
        exit(1);
    }

    // Anillo logico de la shared memory de la planilla general
    ringPids[i] = fork();
    if(ringPids[i++] == 0) {
        execlp(ringProgram.str().c_str(), program.str().c_str(), configPlanillaGeneralShmemFileName.c_str(), paramFirstTime, (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del programa del anillo de la shmem de la planilla general. Algo salio mal!", __FILE__);
        exit(1);
    }
    
    // Anillo logico de la shared memory de la planilla asignacion
    ringPids[i] = fork();
    if(ringPids[i++] == 0) {
        execlp(ringProgram.str().c_str(), program.str().c_str(), configPlanillaAsignacionShmemFileName.c_str(), paramFirstTime, (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del programa del anillo de la shmem de la planilla asignacion. Algo salio mal!", __FILE__);
        exit(1);
    }

    // Espero a que terminen los 3 procesos
    for (int j = 0; j < 3; j++) {
        int status;
        waitpid(ringPids[i], &status, 0);
    }
    /*ANILLOS SUPUESTAMENTE GENERADOS*/
    Logger::notice("Anillos generados! Dejo corriendo los listeners para los rearmados...", __FILE__);
    // Listener para el anillo de la shmem inter-broker
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configBrokerShmemFileName.c_str(), (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del listener del anillo de la shmem inter-broker. Algo salio mal!", __FILE__);
        exit(1);
    }

    // Anillo logico de la shared memory de la planilla general
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configPlanillaGeneralShmemFileName.c_str(), (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del listener del anillo de la shmem de la planilla general. Algo salio mal!", __FILE__);
        exit(1);
    }
    
    // Anillo logico de la shared memory de la planilla asignacion
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configPlanillaAsignacionShmemFileName.c_str(), (char*) 0);
        Logger::error("Se imprime si no se ejecuto el execlp del listener del anillo de la shmem de la planilla asignacion. Algo salio mal!", __FILE__);
        exit(1);
    }
}

int main (int argc, char* argv[]) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::debug("Creando IPCs...", __FILE__);
    crearIpc();
    Logger::debug("Creados todos los IPCs", __FILE__);
	crearServers();
	crearModulosBroker();
    
    armarAnillo();
    lanzarTareasMaster();
    
    return 0;
}
