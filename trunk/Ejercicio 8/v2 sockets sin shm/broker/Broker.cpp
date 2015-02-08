#include <stdio.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"

void crearIpc() {

	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	msgget(key, IPC_CREAT | 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	msgget(key, IPC_CREAT | 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	msgget(key, IPC_CREAT | 0660);

	key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTablaTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles) ,IPC_CREAT | 0660);
    TTablaIdTestersDisponibles* tablaTesterComun = (TTablaIdTestersDisponibles*) shmat(shmTablaTestCom, NULL, 0);
    tablaTesterComun->ultimoTesterElegido = 0;

    key = ftok(ipcFileName.c_str(), SHM_TESTERS_ESPECIALES_DISPONIBLES);
    int shmTablaTestEsp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles) ,IPC_CREAT | 0660);
    TTablaIdTestersEspecialesDisponibles* tablaTesterEsp = (TTablaIdTestersEspecialesDisponibles*) shmat(shmTablaTestEsp, NULL, 0);
    tablaTesterEsp->ultimoTesterElegido = 0;

    Semaphore semTablaCom(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    semTablaCom.getSem();
    semTablaCom.iniSem(1);
    
    Semaphore semTablaEsp(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    semTablaEsp.getSem();
    semTablaEsp.iniSem(1);

    for (int i = 0; i < MAX_TESTER_ESPECIALES; i++) {
		Semaphore semEspecial(i + ID_TESTER_ESP_START);
		semEspecial.creaSem();
		semEspecial.iniSem(0);
	}
}

void crearModulosBroker() {

	Logger::notice("Creo el servidor rpc", __FILE__);
	if (fork() == 0){
		execlp("./bin/idServer", "idServer", (char*)0);
		Logger::notice ("Mensaje luego de execlp de idServer. Algo salio mal!", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el modulo de broker pasamanos emisor", __FILE__);
	if (fork() == 0){
		execlp("./bin/brokerPasamanosEmisor", "brokerPasamanosEmisor", (char*)0);
		Logger::notice ("Mensaje luego de execlp de brokerPasamanosEmisor. Algo salio mal!", __FILE__);
        exit(1);
	}
    
    Logger::notice("Creo el modulo de broker pasamanos receptor", __FILE__);
	if (fork() == 0){
		execlp("./bin/brokerPasamanosReceptor", "brokerPasamanosReceptor", (char*)0);
		Logger::notice ("Mensaje luego de execlp de brokerPasamanosReceptor. Algo salio mal!", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el modulo de broker de nuevos requerimientos de dispositivos", __FILE__);
	if (fork() == 0){
		execlp("./broker/brokerRequerimientosDisp", "brokerRequerimientosDisp", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerRequerimientosDisp. Algo salio mal!", __FILE__);
        exit(1);
	}
	
	Logger::notice("Creo el modulo de broker de requerimientos para testers especiales", __FILE__);
	if (fork() == 0){
		execlp("./broker/brokerReqTestEsp", "brokerReqTestEsp", (char*)0);
        Logger::notice ("Mensaje luego de execlp de brokerReqTestEsp. Algo salio mal!", __FILE__);
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
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR , paramMsgQueue,(char*)0);
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
    /*
    // Comunicacion con tester especiales
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPTOR_TESTERS_ESPECIALES);
    Logger::notice("Creo el servidor receptor de mensajes de testers especiales", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_TESTERS_ESPECIALES , paramMsgQueue, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el servidor emisor de mensajes a testers", __FILE__);
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_EMISOR_TESTERS_ESPECIALES);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_TESTERS_ESPECIALES , paramMsgQueue,(char*)0);
        exit(1);
	}
    
    // Comunicacion con equipo especial
    sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_RECEPTOR_EQUIPO_ESPECIAL);
    Logger::notice("Creo el servidor receptor de mensajes de testers especiales", __FILE__);
	if (fork() == 0){
		execlp("./tcp/tcpserver_receptor", "tcpserver_receptor", PUERTO_SERVER_RECEPTOR_EQUIPO_ESPECIAL , paramMsgQueue, (char*)0);
        exit(1);
	}
	
	Logger::notice("Creo el servidor emisor de mensajes a testers", __FILE__);
	sprintf(paramMsgQueue, "%d", MSGQUEUE_BROKER_EMISOR_EQUIPO_ESPECIAL);
	if (fork() == 0){
		execlp("./tcp/tcpserver_emisor", "tcpserver_emisor", PUERTO_SERVER_EMISOR_EQUIPO_ESPECIAL , paramMsgQueue,(char*)0);
        exit(1);
	}
*/
}

int main (int argc, char* argv[]) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	crearIpc();
	crearServers();
	crearModulosBroker();
    return 0;
}