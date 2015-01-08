#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../logger/Logger.h"

int main(int argc, char** argv){
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	Logger::notice("Creo las colas necesarias", __FILE__);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_desde_dispositivos = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS_FINAL);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_GETTING_IDS);
    int cola_ids = msgget(key, 0666);
	
	int id_broker = atoi(argv[1]);
	
	/* Fin setup */
	TMessageAtendedor msg;
	
	while (true){
		Logger::notice("Espero un mensaje hacia un tester", __FILE__);
		int ok_read = msgrcv(cola_desde_dispositivos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0); //LEO TODOS
		if (ok_read == -1){
			exit(0);
		}
		
		int id_tester = msg.mtype;
		if (id_tester > MAX_DISPOSITIVOS_EN_SISTEMA){
			Logger::notice("Llega un mensaje de uso interno del broker. Despacho inmediatamente", __FILE__);
			int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
			continue;
		}
		std::stringstream ss;
		ss << "Llego un mensaje para el tester " << id_tester;
		Logger::notice(ss.str(), __FILE__);
		ss.str("");	
				
		int id_broker_of_tester;
		// Uso cliente rpc
		if (fork() == 0){
			char param_tester[4];
			sprintf(param_tester, "%d",id_tester);
			execlp("./servicio_rpc/consultar_broker_tester", "consultar_broker_tester", UBICACION_SERVER , param_tester ,(char*)0);
			printf("ALGO NO ANDUVO\n");
			exit(1);
		}
		wait(NULL);
		
		TMessageAtendedor msg_id;
		ok_read = msgrcv(cola_ids, &msg_id, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_CONSULTA_BROKER, 0);
		if (ok_read == -1){
			//No se pudo conseguir id
			exit(-1);
		}
	
		id_broker_of_tester = msg_id.value;
	
		if (id_broker_of_tester <= 0){
			//Algo salio mal, el tester no se registro todavia pero hay alguien mandandole mensajes... uuuiiiiuuuuuu
			exit(id_broker_of_tester);
		}
				
		// reviso si el tester pertenece a mi broker
		if (id_broker_of_tester != id_broker){
			ss << "El tester pertenecia a otro broker. Yo soy broker " << id_broker << " Y el esta en broker " << id_broker_of_tester << ". Cambio mtypes para que se envie bien";
			Logger::notice(ss.str(), __FILE__);
			ss.str("");	
			msg.mtype_envio = id_tester;
			msg.mtype = id_broker_of_tester;
		} else {
			ss << "El tester pertenecia esta conectado a este broker (" << id_broker_of_tester << ") asi que hago de pasamanos";
			Logger::notice(ss.str(), __FILE__);
			ss.str("");	
		}
		
		int ret = msgsnd(cola_hacia_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if(ret == -1) {
			exit(1);
		}		
	}
}
