#include <cstdlib>
#include <sys/msg.h>
#include "errno.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../logger/Logger.h"
#include "../common/common.h"


using namespace std;

int main(int argc, char** argv) {
	int id = atoi(argv[1]);
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id ;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
	
	int id_cola = atoi(argv[2]);
	
    key_t key = ftok(ipcFileName.c_str(), id_cola);
    int cola_lectura = msgget(key, 0660);
    
    //SOCKET ESCRITURA
   
	Logger::notice("Procesamiento inicial completo, ejecutando ciclo principal" , nombre.str().c_str());
	
	size_t tam_mensaje = (size_t) atoi(argv[3]);
	void* msg = malloc(tam_mensaje);
	
    while (true){
        int ok_read = msgrcv(cola_lectura,msg , tam_mensaje - sizeof(long), 0, 0);
        if (ok_read == -1){
			exit(0);
		}
		Logger::notice("Hay que enviar mensaje" , nombre.str().c_str());
				
		//ENVIAR POR EL SOCKET!
		
		Logger::notice("Envie mensaje por socket", nombre.str().c_str());
    }
    free(msg);
    return 0;
}

