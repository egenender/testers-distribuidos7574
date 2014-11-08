#include "Planilla.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"

Planilla::Planilla(): mutex_resul(SEM_PLANILLA_GENERAL){
	key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmlocalid = shmget(key, sizeof(resultado_t) * CANT_RESULTADOS, 0660);
    this->resultados = (resultado_t*)shmat(shmlocalid, NULL, 0);
    mutex_resul.getSem();

}

Planilla::~Planilla() {

}


bool lugar_desocupado(resultado_t resul){
	return resul.terminado;
}

int Planilla::obtenerLugar(){
	for (int i = 0; i < CANT_RESULTADOS; i++){
		if (lugar_desocupado(resultados[i]))
			return i;
	}
	
	return -1;
}

int Planilla::setRequerimiento(int idDispositivo, bool testers[]){
	mutex_resul.p();
	int lugar = this->obtenerLugar();
	this->resultados[lugar].idDispositivo = idDispositivo;
	this->resultados[lugar].resultadosPendientes = 0;
	this->resultados[lugar].resultadosGraves = 0;
	this->resultados[lugar].num_testeos = 0;
	this->resultados[lugar].terminado = false;
	
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
		if (testers[i]){
			this->resultados[lugar].resultadosPendientes++;
		}
		this->resultados[lugar].testers_involucrados[i] = testers[i];
	}
	
	mutex_resul.v();
	return lugar;
}

int Planilla::terminarProcesamiento(int idTesterEspecial, int req, bool resulGrave){
	int devolucion;
	mutex_resul.p();
	this->resultados[req].resultadosPendientes--;
	if (resulGrave){
		this->resultados[req].resultadosGraves++;
	}
	if (this->resultados[req].resultadosPendientes != 0){
		mutex_resul.v();
		std::stringstream ss;
		ss << "El tester especial " << idTesterEspecial << " se queda esperando a que se terminen los testeos especiales en el semaforo "<< (idTesterEspecial - ID_TESTER_ESPECIAL_START);
		Logger::notice(ss.str(), __FILE__);
		ss.str("");
		Semaphore sem_tester(SEM_TESTERS_ESPECIALES + (idTesterEspecial - ID_TESTER_ESPECIAL_START));
		sem_tester.getSem();
		sem_tester.p();
		
		ss << "El tester especial " << idTesterEspecial << " se despierta. Se terminaron los testeos especiales";
		Logger::notice(ss.str(), __FILE__);
		
		mutex_resul.p();
		if (this->resultados[req].terminado){
			devolucion = NO_CONTESTAR;
		}else{
			devolucion = REPETIR_TEST;
		}
		mutex_resul.v();
	}else{
		Logger::notice("Se termino el testeo", __FILE__);
		this->resultados[req].num_testeos++;
		if (this->resultados[req].resultadosGraves == 0){
			devolucion = RESULTADO_NO_GRAVE;
			this->resultados[req].terminado = true;
		}else{
			if (this->resultados[req].num_testeos == MAX_REPETICIONES_TESTEOS){
				Logger::notice("Ya se hicieron la maxima cantidad de testeos, asi que no se resuelve el error", __FILE__);
				this->resultados[req].terminado = true;
				devolucion = RESULTADO_GRAVE;
			}else{
				Logger::notice("Se puede seguir testeando, asi que seguimos intentado", __FILE__);
				this->resultados[req].resultadosGraves = 0;
				this->resultados[req].resultadosPendientes = 0; //por cualquier cosa
				for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
					if (this->resultados[req].testers_involucrados[i])
						this->resultados[req].resultadosPendientes++;
				}
				devolucion = SEGUIR_TESTEANDO;
			}
			
		}
		
		for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
			if (this->resultados[req].testers_involucrados[i] && (i + ID_TESTER_ESPECIAL_START) != idTesterEspecial ){
				std::stringstream ss2;
				ss2 << "El testers Especial " << idTesterEspecial << " va a despertar al tester especial " << i + ID_TESTER_ESPECIAL_START << "Que espera en el semaforo " << i;
				Logger::notice(ss2.str(), __FILE__);
				Semaphore sem_tester(SEM_TESTERS_ESPECIALES + i);
				sem_tester.getSem();
				sem_tester.v();
			}
		}
		mutex_resul.v();	
	}
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
