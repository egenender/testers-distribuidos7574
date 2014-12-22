#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include "../common/common.h"
#include "../logger/Logger.h"

int main(void){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo las colas necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_MENSAJES_DISPOSITIVOS);
	int cola_desde_dispositivos = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ATENCION_REQUERIMIENTOS_DISPOSITIVOS);
	int cola_at_req = msgget(key, 0660 );
	
	/* Fin setup */
	TMessageAtendedor msg;
	
	while (true){
		Logger::notice("Espero un mensaje de un dispositivo", __FILE__);
		int ok_read = msgrcv(cola_desde_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0); //LEO TODOS
		if (ok_read == -1){
			exit(0);
		}
		std::stringstream ss;
		ss << "Llego un mensaje del dispositivo " << msg.idDispositivo;
		Logger::notice(ss.str(), __FILE__);
		if (msg.es_requerimiento){
			Logger::notice("Era un requerimiento, asi que se lo mando al atendedor de mensajes de requerimientos", __FILE__);
			int ret = msgsnd(cola_at_req, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
		}else {
			Logger::notice("No era un requerimiento, asi que hago de pasamanos y se lo paso a la cola de mensajes hacia testers", __FILE__);
			int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
		}		
	}
}
