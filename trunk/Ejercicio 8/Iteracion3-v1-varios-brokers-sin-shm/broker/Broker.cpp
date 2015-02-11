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
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	msgget(key, IPC_CREAT | 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
	msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_BROKER_TESTERS_REGISTRADOS);
    int shmTablaTestCom = shmget(key, sizeof(TTablaBrokerTestersRegistrados), IPC_CREAT | 0660);
    TTablaBrokerTestersRegistrados* tablaTesterRegistrados = (TTablaBrokerTestersRegistrados*) shmat(shmTablaTestCom, NULL, 0);
    for (int i = 0; i < MAX_TESTER_COMUNES + MAX_TESTER_ESPECIALES; i++)    tablaTesterRegistrados->registrados[i] = false;
    tablaTesterRegistrados->ultimoTesterElegido = 0;

    Semaphore semTestersRegistrados(SEM_BROKER_TESTERS_REGISTRADOS);
    semTestersRegistrados.creaSem();
    semTestersRegistrados.iniSem(1);

    Semaphore semEspecialAsignacion(SEM_ESPECIALES_ASIGNACION);
    semEspecialAsignacion.creaSem();
    semEspecialAsignacion.iniSem(1);

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

}

void crearServers(){
	
    char paramMsgQueue[10];
    
    // Comunicacion con testers y equipo especial
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPTOR);
    Logger::notice("Creo el servidor receptor de mensajes de testers comunes", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR , paramMsgQueue, (char*)0);
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
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_DISPOSITIVOS , paramMsgQueue, (char*)0);
        exit(1);
	}
	
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	Logger::notice("Creo el servidor emisor de mensajes a dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_DISPOSITIVOS , paramMsgQueue,(char*)0);
        exit(1);
	}
}

int main (int argc, char* argv[]) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crearIpc();
	crearServers();
	crearModulosBroker();
    return 0;
}