#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include <string.h>

void copiarResultado(resultado_t* destino, resultado_t* origen){
	for (int i = 0; i < CANT_RESULTADOS; i++){
		destino[i].idDispositivo = origen[i].idDispositivo;
		destino[i].resultadosPendientes = origen[i].resultadosPendientes;
		destino[i].resultadosGraves = origen[i].resultadosGraves;
	}
}


int main (void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo los ipcs necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS);
	int cola_requerimiento_shm = msgget(key, 0660);
	
	
	resultado_t resultados[CANT_RESULTADOS];
	for (int i = 0; i < CANT_RESULTADOS; i++){
		resultados[i].resultadosPendientes = 0;
		resultados[i].idDispositivo = 0;
		resultados[i].resultadosGraves = 0;
    }
	    
    Logger::notice("Termino la obtencion de ipcs", __FILE__);
    
	/* Fin Setup */
	TMessageAtendedor msg;
	
	while (true){
		Logger::notice("Espero por un requerimiento de shm", __FILE__);
		int ok_read = msgrcv(cola_requerimiento_shm, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO_SHM_TESTERS, 0);
		if (ok_read == -1){
			exit(0);
		}
		
		std::stringstream ss;
		ss << "Me llega un requerimiento de shm desde tester " << msg.tester;
		Logger::notice(ss.str(), __FILE__);
		
		/* Le doy la shm */
		msg.mtype = msg.tester;
		//memcpy(msg.resultados, resultados, sizeof(resultados));
		copiarResultado(msg.resultados, resultados);
		/* */
		int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if(ret == -1) {
			exit(1);
		}
		ss.str("");
		ss << "Envie shm a tester " << msg.tester << ". Espero que me la devuelva";
		Logger::notice(ss.str(), __FILE__);
		
		ok_read = msgrcv(cola_requerimiento_shm, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_DEVOLUCION_SHM_TESTERS, 0);
		if (ok_read == -1){
			exit(0);
		}
		ss.str("");
		ss << "El tester " << msg.tester << " me devolvio la shm";
		Logger::notice(ss.str(), __FILE__);
		
		/* Actualizo shm */
		//memcpy(resultados, msg.resultados, sizeof(resultados));
		copiarResultado(resultados, msg.resultados);
	}
}

