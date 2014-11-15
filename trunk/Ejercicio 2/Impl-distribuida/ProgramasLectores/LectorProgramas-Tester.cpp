#include <cstdlib>
#include <sys/msg.h>
#include "errno.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../logger/Logger.h"
#include "../common/common.h"


using namespace std;

int main(int argc, char** argv) {
	int idTester = atoi(argv[1]);
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << idTester ;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
	
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_PROGRAMAS_ENVIO);
    int cola_lectura = msgget(key, 0660);
    key = ftok(ipcFileName.c_str(), MSGQUEUE_PROGRAMAS_SOCKET);
    int cola_escritura = msgget(key, 0660);
   
   Logger::notice("Procesamiento inicial completo, ejecutando ciclo principal" , nombre.str().c_str());
   
    while (true){
		TMessageAtendedor msg;
        int ok_read = msgrcv(cola_lectura, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
        if (ok_read == -1){
			exit(0);
		}
		std::stringstream ss;
		ss << "Se recibe nuevo programa desde tester " << msg.idDispositivo;
		Logger::notice(ss.str() , nombre.str().c_str());
				
		int ok = msgsnd(cola_escritura, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
        if (ok == -1){
			exit(0);
		}	
		Logger::notice("Puse el resultado en la cola correcta para que lo lea el lector del otro lado correspondiente", nombre.str().c_str());
    }
    return 0;
}

