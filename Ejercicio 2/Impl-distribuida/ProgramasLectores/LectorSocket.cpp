#include <cstdlib>
#include <sys/msg.h>
#include "errno.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../common/common.h"
#include "../logger/Logger.h"


using namespace std;

int main(int argc, char** argv) {
	int id = atoi(argv[1]);
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id ;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
	
	int id_cola = atoi(argv[2]);
	
    //SOCKET LECTURA
    
    key = ftok(ipcFileName.c_str(), id_cola);
	int cola_escritura = msgget(key, 0660);
   
	Logger::notice("Procesamiento inicial completo, ejecutando ciclo principal" , nombre.str().c_str());
	
	size_t tam_lectura = (size_t) atoi(argv[3]);
	void* msg = malloc (tam_lectura);
	
	while (true){	
		//LEER DESDE EL SOCKET!
		
		Logger::notice("Se recibe un mensaje" , nombre.str().c_str());
				
		int ok = msgsnd(cola_escritura, msg, tam_lectura - sizeof(long), 0);
        if (ok == -1){
			exit(0);
		}	
		
		Logger::notice("Puse el mensaje en la cola correcta", nombre.str().c_str());
    }
    free(msg);
    return 0;
}

