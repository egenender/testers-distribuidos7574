/* 
 * File:   brokerRegistroTesters.cpp
 * Author: knoppix
 *
 * Created on February 9, 2015, 10:44 PM
 */

#include <cstdlib>
#include <sys/msg.h>
#include <sys/shm.h>

#include "logger/Logger.h"
#include "ipc/Semaphore.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
	int msgQueueRegistros = msgget(key, 0660);

    key = ftok(ipcFileName.c_str(), SHM_BROKER_TESTERS_REGISTRADOS);
    int shmTablaTestersRegistrados = shmget(key, sizeof(TTablaBrokerTestersRegistrados), IPC_CREAT | 0660);
    TTablaBrokerTestersRegistrados* tablaTestersRegistrados = (TTablaBrokerTestersRegistrados*) shmat(shmTablaTestersRegistrados, (void*) NULL, 0);
    
    Semaphore semTabla(SEM_BROKER_TESTERS_REGISTRADOS);
    semTabla.getSem();
    
    while(true) {

        TMessageAtendedor msg;
        Logger::notice("Espero por un pedido de registro...", __FILE__);
		int okRead = msgrcv(msgQueueRegistros, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
			exit(0);
		}

		std::stringstream ss;
		ss << "Me llego un pedido de requerimiento desde el tester " << msg.tester;
		Logger::notice(ss.str(), __FILE__);
        
        // Calculo el ID en la tabla de testers registrados en base a si es 
        // Tester Comun -> Al ppio de la tabla; Tester Especial -> al final de la tabla
        int id = msg.tester;
        if (msg.esTesterEspecial) {
            id = id - ID_TESTER_ESP_START + MAX_TESTER_COMUNES;
        } else  id -= ID_TESTER_COMUN_START;
        
        semTabla.p();
        if (!tablaTestersRegistrados->registrados[id]) {
            tablaTestersRegistrados->registrados[id] = true;
            tablaTestersRegistrados->disponibles[id] = true;

            if (msg.esTesterEspecial) {
                // Levanto el semaforo por si hay un requerimiento especial esperando
                Semaphore semEspecial(SEM_ESPECIALES + id - MAX_TESTER_COMUNES);
                semEspecial.getSem();
                semEspecial.v();
            }
            
            std::stringstream ss;
            ss << "Se ha registrado al tester " << msg.tester << " con exito";
            Logger::notice(ss.str(), __FILE__);

        } else {
            std::stringstream ss;
            ss << "El tester " << msg.tester << " ya se encontraba registrado en el sistema!";
            Logger::error(ss.str(), __FILE__);
            // TODO: ¿Que hacemos aca?
        }
        semTabla.v();
        
    }

    return 0;
}

