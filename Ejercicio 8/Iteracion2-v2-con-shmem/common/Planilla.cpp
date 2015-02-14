/* 
 * File:   Planilla.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:08 PM
 */

#include "Planilla.h"
#include "logger/Logger.h"

Planilla::Planilla(int idTester) : idTester(idTester) {

    // Obtengo la msgqueue de envio y recepcion de shmem
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL);
	this->shmemMsgqueueEmisor = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueEmisor == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL);
	this->shmemMsgqueueReceptor = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueReceptor == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS);
	this->shmemMsgqueueReq = msgget(key, IPC_CREAT | 0660);
    if(this->shmemMsgqueueReq == -1) {
        Logger::error("Error al construir la msgqueue para la gestion de la memora compartida distribuida", __FILE__);
        exit(1);
    }

    // Creo la comunicacion al broker para la memoria compartida distribuida
    char paramIdCola[10];
    char paramId[10];
    char paramSize[10];

	sprintf(paramIdCola, "%d", MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL);
    sprintf(paramId, "%d", this->idTester + INIT_MTYPE_SHMEM_PLANILLA_GENERAL);
    sprintf(paramSize, "%d", (int) sizeof(TSharedMemoryPlanillaGeneral));

	this->pidReceptor = fork();
	if (this->pidReceptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_ENVIO_SHM_PLANILLA_GENERAL,
                paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
		exit(1);
	}

    sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL);
    sprintf(paramId, "%d", 0); // Para que envie todos los mensajes

    this->pidEmisor = fork();
	if (this->pidEmisor == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPCION_SHM_PLANILLA_GENERAL,
				paramId, paramIdCola, paramSize,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}
}

Planilla::Planilla(const Planilla& orig) {
}

Planilla::~Planilla() {
}

int Planilla::hayLugar() {

    // Si hay lugar, registro el dispositivo
    int result = SIN_LUGAR;
    // Obtengo la memoria compartida
    this->obtenerSharedMemory();
    if(this->memoria.cantDispositivosSiendoTesteados >= MAX_DISPOSITIVOS_EN_SISTEMA) {
    } else {
        this->memoria.cantDispositivosSiendoTesteados += 1;
        int i = 0;
        for (i = 0; (i < MAX_DISPOSITIVOS_EN_SISTEMA) && (this->memoria.idsPrivadosDispositivos[i]); i++);
        this->memoria.idsPrivadosDispositivos[i] = true;
        result = i;
    }
    this->devolverSharedMemory();
    return result;
}
void Planilla::eliminarDispositivo(int posicionDispositivo) {

    this->obtenerSharedMemory();
    this->memoria.cantDispositivosSiendoTesteados -= 1;
    this->memoria.idsPrivadosDispositivos[posicionDispositivo] = false;
    this->devolverSharedMemory();
}

void Planilla::obtenerSharedMemory() {
    TRequerimientoSharedMemory req;
    req.mtype = MTYPE_REQ_SHMEM_PLANILLA_GENERAL;
    req.idDevolucion = this->idTester + INIT_MTYPE_SHMEM_PLANILLA_GENERAL;
    req.idSolicitante = this->idTester;
    int okSend = msgsnd(this->shmemMsgqueueReq, &req, sizeof(TRequerimientoSharedMemory) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error pidiendo la shmem distribuida de planilla general para el tester " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    // Espero por la shmem
    int okRead = msgrcv(this->shmemMsgqueueReceptor, &this->memoria, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), this->idTester + INIT_MTYPE_SHMEM_PLANILLA_GENERAL, 0);
    if(okRead == -1) {
        std::stringstream ss;
        ss << "Error leyendo de la msgqueue la shmem distribuida de planilla general para el tester " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
    std::stringstream ss; ss << "Me llega la shmem de planilla general al tester/equipo " << this->memoria.mtype;
    //Logger::debug(ss.str(), __FILE__);
}

void Planilla::devolverSharedMemory() {
    std::stringstream ss; ss << "Tester/equipo " << this->idTester << " envía planilla general de vuelta";
    //Logger::debug(ss.str(), __FILE__);
    this->memoria.mtype = MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL;
    int okSend = msgsnd(this->shmemMsgqueueEmisor, &this->memoria, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
    if(okSend == -1) {
        std::stringstream ss;
        ss << "Error devolviendo la shmem distribuida de planilla general desde el tester " << this->idTester;
        Logger::error(ss.str(), __FILE__);
        exit(1);
    }
}