#include "Planilla.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../ipc/Semaphore.h"


Planilla::Planilla(): mutex_resul(SEM_PLANILLA_GENERAL){
	key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmlocalid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS, 0660);
    this->resultados = (resultado_t*)shmat(shmlocalid, NULL, 0);
    mutex_resul.getSem();
}

Planilla::Planilla(const Planilla& orig) : mutex_resul(SEM_PLANILLA_GENERAL){
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
	mutex_resul.v();
	
	return -1;
}

int Planilla::setRequerimiento(int idDispositivo, int cant_resultados_necesarios){
	mutex_resul.p();
	int lugar = this->obtenerLugar();
	this->resultados[lugar].idDispositivo = idDispositivo;
	this->resultados[lugar].resultadosPendientes = cant_resultados_necesarios;
	this->resultados[lugar].resultadosGraves = 0;
	mutex_resul.v();
	return lugar;
}

int Planilla::terminarProcesamiento(int req, bool resulGrave){
	int devolucion;
	mutex_resul.p();
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
	mutex_resul.v();
	
	return devolucion;
}

int Planilla::dispositivoEnLugar(int lugar){
	mutex_resul.p();
	int idDispositivo = this->resultados[lugar].idDispositivo;
	mutex_resul.v();
	return idDispositivo;
}

bool Planilla::hayLugar(){
	mutex_resul.p();
	bool rta = this->obtenerLugar() != -1;
	mutex_resul.v();
	return rta;
}
