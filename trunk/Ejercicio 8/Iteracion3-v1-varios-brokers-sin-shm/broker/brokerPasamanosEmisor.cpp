#include <iostream>
#include <sys/msg.h>
#include <cstdlib>
#include <unistd.h>
#include "common/common.h"
#include "logger/Logger.h"

int main(int argc, char* argv[]) {

	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPTOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTestYEq = msgget(key, 0660);
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	int msgQueueReqDisp = msgget(key, 0660);
    
    // Queue hacia donde se envian mensajes de otros brokers
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_HACIA_BROKER);
	int msgQueueHaciaBrokers = msgget(key, IPC_CREAT | 0660);
	
	TMessageAtendedor msg;
    int ret = 0;
    std::stringstream ss;
	while(true) {
		Logger::notice("Espero un mensaje de un dispositivo", __FILE__);
		int okRead = msgrcv(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
            Logger::error("Error al recibir un mensaje de la cola del dispositivo", __FILE__);
			exit(1);
		}
        
        switch(msg.mtypeMensaje) {
        
            case MTYPE_REQUERIMIENTO_DISPOSITIVO:
                ss << "Llego un requerimiento del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ret = msgsnd(msgQueueReqDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if(ret == -1) {
                    Logger::error("Error al enviar el mensaje a la cola de requerimientos de dispositivo");
                    exit(1);
                }
                break;
            
            case MTYPE_RESULTADO_INICIAL:
                ss << "Llego resultado de test inicial del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ss << "El resultado inicial debe ser enviado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                
                if (msg.idBroker == ID_BROKER) {
                    // El mensaje es de para tester en este broker
                    msg.mtype = msg.tester;
                    ret = msgsnd(msgQueueTestYEq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio a testers comunes", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío resultado inicial hacia el broker " << msg.idBroker;
                    Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // El mensaje es para tester en otro broker
                    msg.mtype = msg.idBroker;
                    msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el mensaje a la cola de envio hacia brokers", __FILE__);
                        exit(1);
                    }
                }
                break;

            case MTYPE_RESULTADO_ESPECIAL:
                std::stringstream ss;
                ss << "Llego resultado de test especial del dispositivo " << msg.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                ss.clear();
                
                ss << "El resultado especial debe ser enviado al broker " << msg.idBroker << " y estoy en el broker " << ID_BROKER;
                Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();

                if (msg.idBroker == ID_BROKER) {
                    msg.mtype = ID_EQUIPO_ESPECIAL;
                    int ret = msgsnd(msgQueueTestYEq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el resultado especial a la cola de envio a testers y equipo especial", __FILE__);
                        exit(1);
                    }
                } else {
                    ss << "Envío resultado especial hacia el broker " << msg.idBroker;
                    Logger::notice(ss.str(), __FILE__); ss.str(""); ss.clear();
                    // El mensaje es para tester en otro broker
                    msg.mtype = msg.idBroker;
                    msg.mtypeMensajeBroker = MTYPE_HACIA_TESTER;
                    ret = msgsnd(msgQueueHaciaBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                    if(ret == -1) {
                        Logger::error("Error al enviar el resultado especial a la cola de envio hacia brokers", __FILE__);
                        exit(1);
                    }
                }
                break;
        }
	}
    return 0;
}