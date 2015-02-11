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

#include "common/common.h"
#include "logger/Logger.h"

using namespace std;

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR);
	int msgQueueReceptor = msgget(key, 0660);

	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_TESTER_ESPECIAL);
	int msgQueueReqTestEsp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REGISTRO_TESTERS);
    int msgQueueRegistroTesters = msgget(key, 0660);

    // Queue hacia donde se envian mensajes de otros brokers
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_HACIA_BROKER);
	int msgQueueHaciaBrokers = msgget(key, IPC_CREAT | 0660);
	
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

            default:
                ss << "Llego mensaje para el dispositivo " << msg.idDispositivo << " que esta conectado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();

                if (msg.idBroker == ID_BROKER) {
                    msg.mtype = msg.idDispositivo;
                    ret = msgsnd(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a dispositivos", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío el mensaje al broker correspondiente de ID " << msg.idBroker;
                    Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // Envio el mensaje al broker correspondiente
                    msg.mtype = msg.idBroker;
                    msg.mtypeMensajeBroker = MTYPE_HACIA_DISPOSITIVO;
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a otros brokers", __FILE__);
                        exit(1);
                    }
                }
                break;
            
        }
	}
    return 0;
}

