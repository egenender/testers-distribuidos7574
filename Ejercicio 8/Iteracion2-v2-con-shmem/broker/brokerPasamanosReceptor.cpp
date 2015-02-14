/* 
 * File:   brokerPasamanosReceptor.cpp
 * Author: ferno
 *
 * Created on February 6, 2015, 1:43 AM
 */

#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "common/common.h"
#include "logger/Logger.h"
#include "ipc/Semaphore.h"

using namespace std;

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	int msgQueueReceptor = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueEmisor = msgget(key, 0660);

	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	int msgQueueReqTestEsp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
    int msgQueueRegistroTesters = msgget(key, 0660);

    key = ftok(ipcFileName.c_str(), SHM_BROKER_TESTERS_REGISTRADOS);
    int shmTablaTestersRegistrados = shmget(key, sizeof(TTablaBrokerTestersRegistrados), IPC_CREAT | 0660);
    TTablaBrokerTestersRegistrados* tablaTestersRegistrados = (TTablaBrokerTestersRegistrados*) shmat(shmTablaTestersRegistrados, (void*) NULL, 0);

    Semaphore semTabla(SEM_BROKER_TESTERS_REGISTRADOS);
    semTabla.getSem();
	
	TMessageAtendedor msg;
    int ret = 0;
    std::stringstream ss;
	while(true) {
		Logger::notice("Espero un mensaje de un dispositivo", __FILE__);
		int okRead = msgrcv(msgQueueReceptor, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
            Logger::error("Error al recibir un mensaje de la cola del dispositivo", __FILE__);
			exit(1);
		}
        
        switch(msg.mtypeMensaje) {
        
            case MTYPE_REQUERIMIENTO_TESTER_ESPECIAL:
                ss << "Llego un requerimiento del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ret = msgsnd(msgQueueReqTestEsp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de requerimientos de dispositivo");
                    exit(1);
                }
                break;

            case MTYPE_REGISTRAR_TESTER:
                ss << "Llego un pedido de registro del tester " << msg.tester;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ret = msgsnd(msgQueueRegistroTesters, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de registros de testers");
                    exit(1);
                }
                break;

            case MTYPE_HAY_QUE_REINICIAR:
                ss << "Llego un mensaje para reiniciar (o no) para el tester especial " << msg.tester;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                msg.mtype = msg.tester;
                ret = msgsnd(msgQueueEmisor, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de registros de testers");
                    exit(1);
                }
                break;

            case MTYPE_AVISAR_DISPONIBILIDAD:
                ss << "Llego un aviso de disponibilidad del tester especial " << msg.tester;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                
                if (fork() == 0) {
                    // Pido la shmem y vuelvo a poner al tester disponible
                    semTabla.p();
                    tablaTestersRegistrados->disponibles[msg.tester - ID_TESTER_ESP_START + MAX_TESTER_COMUNES] = true;
                    semTabla.v();                    
                    exit(0);
                }
                break;

            default:
                ss << "Llego mensaje para el dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                msg.mtype = msg.idDispositivo;
                ret = msgsnd(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de envio a dispositivos");
                    exit(1);
                }
                break;
            
        }
	}
    return 0;
}

