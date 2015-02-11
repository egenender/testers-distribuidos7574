/* 
 * File:   interBrokerMsgHandler.cpp
 * Author: ferno
 *
 * Created on February 11, 2015, 10:12 AM
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/msg.h>

#include "../logger/Logger.h"
#include "../common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << ID_BROKER;

    // Queue desde donde se reciben mensajes de otros brokers
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_DESDE_BROKER);
	int msgQueueDesdeBrokers = msgget(key, IPC_CREAT | 0660);
    
    // Queue hacia donde se envian mensajes a los dispositivos
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueDisp = msgget(key, 0660);
    
    // Queue hacia donde se envian mensajes a los testers
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTester = msgget(key, 0660);
    
    while(true) {

        // Leo todos los mensajes desde un broker y lo mando a cola de testers o a 
        // cola de dispositivos segun corresponda
        TMessageAtendedor msg;
        int okRead = msgrcv(msgQueueDesdeBrokers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
        if (okRead == -1) {
            std::stringstream ss;
            ss << "Error al leer de la cola desde otros brokers. Errno: " << strerror(errno);
            Logger::error(ss.str(), nombre.str().c_str());
            exit(1);
        }
        
        std::stringstream log;

        // Por aca no van a venir requerimientos de dispositivos o de especiales, sino msgs
        // directamente para testers o para dispositivos
        switch(msg.mtypeMensajeBroker) {
        
            case MTYPE_HACIA_DISPOSITIVO: {
                log << "Me llego un mensaje desde el broker " << msg.idBroker << " para enviar al dispositivo " << msg.idDispositivo;
                Logger::notice(log.str(), nombre.str().c_str());

                // Cambio el mtype al ID del dispositivo y envio a la cola hacia disp
                msg.mtype = msg.idDispositivo;
                int okSend = msgsnd(msgQueueDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar mensaje hacia dispositivo " << msg.idDispositivo << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                break;
            }

            case MTYPE_HACIA_TESTER: {
                log << "Me llego un mensaje desde el broker " << msg.idBroker << " para enviar al tester " << msg.tester;
                Logger::notice(log.str(), nombre.str().c_str());

                // Cambio el mtype al ID del tester y envio a la cola hacia tester
                msg.mtype = msg.tester;
                int okSend = msgsnd(msgQueueTester, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar mensaje hacia tester " << msg.tester << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                break;
            }
        }

    }

    return 0;
}

