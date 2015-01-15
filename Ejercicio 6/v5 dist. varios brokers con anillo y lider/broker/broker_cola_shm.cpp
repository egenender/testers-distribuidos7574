#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include "../common/common.h"
#include "../logger/Logger.h"

int main(int argc, char** argv){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo las colas necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_DISPOSITIVOS_FINAL);
	int cola_hacia_dispositivos = msgget(key, 0660 );
	
	int id_espera = atoi(argv[1]);
	int cola_envio = atoi(argv[2]);
	
	key = ftok(ipcFileName.c_str(), cola_envio);
	int cola_hacia_requerimientos = msgget(key, 0660 );
	
	/* Fin setup */
	TMessageAtendedor msg;
	
	while(true){
		Logger::notice("Espero por un mensaje para cambiar de colas", __FILE__);
		int ok_read = msgrcv(cola_hacia_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), id_espera, 0); 
		if (ok_read == -1){
			exit(0);
		}
		
		std::stringstream ss; 
		ss << "Me llega un mensaje por el id " << id_espera << ". Pongo en cola " << cola_envio;
		Logger::notice(ss.str(), __FILE__);
		
		int ret = msgsnd(cola_hacia_requerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if(ret == -1) {
			exit(1);
		}
	}
}
