#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"

const char* IP_BROKERS[CANT_BROKERS] = {"127.0.0.1"/*, "192.168.2.7"*/};

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
	
	for(int i = MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS; i <= MSGQUEUE_RECEPCION_BROKER_SHM; i++) {
        key = ftok(ipcFileName.c_str(), i);
        msgget(key, IPC_CREAT | 0660);
    }    
    
    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shmCantReqBrokerShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* cantReqBrokerShm = (int*) shmat(shmCantReqBrokerShMem, NULL, 0);
    *cantReqBrokerShm = 0;

    Semaphore semEspecialAsignacion(SEM_ESPECIALES_ASIGNACION);
    semEspecialAsignacion.creaSem();
    semEspecialAsignacion.iniSem(1);
    
    Semaphore semReqBrokerShm(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semReqBrokerShm.creaSem();
    semReqBrokerShm.iniSem(1);

    for (int i = 0; i < MAX_TESTER_ESPECIALES; i++) {
		Semaphore semEspecial(i + SEM_ESPECIALES);
		semEspecial.creaSem();
		semEspecial.iniSem(0);
	}
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
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR , paramMsgQueue, (char*)0);
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
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS , paramMsgQueue,(char*) 0);
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

    // Para dar tiempo a que todos los broker esten arriba
	sleep(5);

    // Comunicacion cliente con brokers para mensajes generales
    sprintf(paramIdBroker, "%d", ID_BROKER);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_HACIA_BROKER);
    for (int i = 0; i < CANT_BROKERS; i++) {
        if (i == (ID_BROKER - ID_BROKER_START)) continue;
        Logger::notice("Creo el cliente emisor de mensajes generales a brokers", __FILE__);
        if (fork() == 0){
            execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[i], PUERTO_CONTRA_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
            exit(1);
        }
    }
    
    // Comunicacion cliente con brokers para memoria compartida de los brokers
    sprintf(paramMsgQueue, "%d", MSGQUEUE_ENVIO_BROKER_SHM);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageShMemInterBroker));
    sprintf(paramIdBroker, "%d", 0); // La conexion es solo con el sgte broker por lo que agarra todo lo que esta en la MsgQueue
    Logger::notice("Creo el cliente emisor de memoria compartida a brokers", __FILE__);
    if (fork() == 0){
        execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", IP_BROKERS[ID_BROKER_SIGUIENTE - ID_BROKER_START], PUERTO_CONTRA_BROKERS_SHMEM_BROKERS , paramIdBroker, paramMsgQueue, paramSizeMsg, (char*) 0);
        exit(1);
    }
    
}

void lanzarTareasMaster() {

    // Tengo que inicializar y lanzar la memoria compartida
    TMessageShMemInterBroker msgShmemInterBroker;
    msgShmemInterBroker.mtype = ID_BROKER;

    for (int i = 0; i < MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES; i++) {
        msgShmemInterBroker.memoria.registrados[i] = false;
        msgShmemInterBroker.memoria.brokerAsignado[i] = TESTER_ESPECIAL_NO_ASIGNADO;
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
}

int main (int argc, char* argv[]) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crearIpc();
	crearServers();
	crearModulosBroker();
    
    if (ID_BROKER == MASTER_BROKER) {
        lanzarTareasMaster();
    }
    
    return 0;
}
