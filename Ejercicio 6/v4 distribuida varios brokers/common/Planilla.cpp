#include "Planilla.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "../ipc/Semaphore.h"
#include <string.h>
#include <stdio.h>

void semaforoDistribuido_P(resultado_t* resultados, int tester);
void semaforoDistribuido_V(resultado_t* resultados, int tester);

Planilla::Planilla(int tester){
	/*key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmlocalid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS, 0660);
    this->resultados = (resultado_t*)shmat(shmlocalid, NULL, 0);*/
    //mutex_resul.getSem();
    this->resultados = (resultado_t*)malloc(sizeof(resultado_t) * CANT_RESULTADOS);
    
    this->tester = tester;
}

Planilla::Planilla(const Planilla& orig){
}

Planilla::~Planilla() {
}


bool lugar_desocupado(resultado_t resul){
	return resul.resultadosPendientes == 0;
}

int Planilla::obtenerLugar(){
	for (int i = 0; i < CANT_RESULTADOS; i++){
		if (lugar_desocupado(resultados[i]))
			return i;
	}
	
	return -1;
}

int Planilla::setRequerimiento(int idDispositivo, int cant_resultados_necesarios){
	//mutex_resul.p();
	semaforoDistribuido_P(resultados, tester);
	int lugar = this->obtenerLugar();
	this->resultados[lugar].idDispositivo = idDispositivo;
	this->resultados[lugar].resultadosPendientes = cant_resultados_necesarios;
	this->resultados[lugar].resultadosGraves = 0;
	//mutex_resul.v();
	semaforoDistribuido_V(resultados, tester);
	return lugar;
}

int Planilla::terminarProcesamiento(int req, bool resulGrave){
	int devolucion;
	//mutex_resul.p();
	semaforoDistribuido_P(resultados, tester);
	this->resultados[req].resultadosPendientes--;
	if (resulGrave){
		this->resultados[req].resultadosGraves++;
	}
	if (this->resultados[req].resultadosPendientes != 0){
		devolucion = NO_CONTESTAR;
	}else if (this->resultados[req].resultadosGraves == 0){
		devolucion = RESULTADO_NO_GRAVE;
	}else{
		devolucion = RESULTADO_GRAVE;
	}
	//mutex_resul.v();
	semaforoDistribuido_V(resultados, tester);
	
	return devolucion;
}

int Planilla::dispositivoEnLugar(int lugar){
	//mutex_resul.p();
	semaforoDistribuido_P(resultados, tester);
	int idDispositivo = this->resultados[lugar].idDispositivo;
	//mutex_resul.v();
	semaforoDistribuido_V(resultados, tester);
	return idDispositivo;
}

bool Planilla::hayLugar(){
	//mutex_resul.p();
	semaforoDistribuido_P(resultados, tester);
	bool rta = this->obtenerLugar() != -1;
	//mutex_resul.v();
	semaforoDistribuido_V(resultados, tester);
	return rta;
}


void copiarResultado(resultado_t* destino, resultado_t* origen){
	for (int i = 0; i < CANT_RESULTADOS; i++){
		destino[i].idDispositivo = origen[i].idDispositivo;
		destino[i].resultadosPendientes = origen[i].resultadosPendientes;
		destino[i].resultadosGraves = origen[i].resultadosGraves;
	}
}


void semaforoDistribuido_P(resultado_t* resultados, int tester){
	key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_RECIBOS);
	int cola_recibos = msgget(key, 0666);
        
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_ENVIOS);
    int cola_envios = msgget(key, 0666);
    	
	TMessageAtendedor msg;
	msg.mtype = tester;
	msg.mtype_envio = MTYPE_REQUERIMIENTO_SHM_TESTERS;
	msg.finalizar_conexion = 0;
	msg.es_requerimiento = 1;
	msg.tester = tester;
	
	msgsnd(cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
        
    msgrcv(cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), tester, 0);
    
    //memcpy(resultados, msg.resultados, sizeof(msg.resultados));
    copiarResultado(resultados, msg.resultados);
}

void semaforoDistribuido_V(resultado_t* resultados, int tester){
	TMessageAtendedor msg;
	msg.mtype = tester;
	msg.mtype_envio = MTYPE_DEVOLUCION_SHM_TESTERS;
	msg.finalizar_conexion = 0;
	msg.es_requerimiento = 1;
	msg.tester = tester;
	//memcpy(msg.resultados, resultados, sizeof(resultados));
	copiarResultado(msg.resultados, resultados);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_ENVIOS);
    int cola_envios = msgget(key, 0666);
   
	msgsnd(cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);   
}

