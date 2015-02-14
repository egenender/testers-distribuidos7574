/* 
 * File:   DistribuidorMensajesTestersComunes.cpp
 * Author: ferno
 *
 * Created on February 8, 2015, 8:55 PM
 */

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "logger/Logger.h"
#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::out);
    ipcFile.close();
    
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL);
    int msgQueueTodos = msgget(key, 0666);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_REQ_TESTER_ESPECIAL);
    int msgQueueReq = msgget(key, 0666);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_REINICIO_TESTER_ESPECIAL);
    int msgQueueReinicio = msgget(key, 0666);
    
    while(true) {
        
        Logger::debug("Busco mensaje para distribuir a los testers especiales", __FILE__);
    
        TMessageAtendedor msg;
        int okRead = msgrcv(msgQueueTodos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
        if (okRead == -1) {
            std::stringstream ss;
            ss << "Error al leer un mensaje para el tester especial para distribuir. Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        
        // Si llega un requerimiento lo envio a la cola de requerimientos, sino a la cola de reinicio
        if(msg.mtypeMensaje == MTYPE_REQUERIMIENTO_TESTER_ESPECIAL) {
            std::stringstream log;
            log << "Distribuyo mensaje de requerimiento del tester " << msg.tester << " para el tester " << msg.mtype << " para testear al dispositivo " << msg.idDispositivo;
            Logger::notice(log.str(), __FILE__); log.str(""); log.clear();
            
            int okSend = msgsnd(msgQueueReq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al distribuir un requerimiento especial a la msgqueue. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
        } else {
            std::stringstream log;
            log << "Distribuyo mensaje de reinicio del Equipo Especial para el tester " << msg.mtype;
            Logger::notice(log.str(), __FILE__); log.str(""); log.clear();
            
            msg.mtype = msg.tester;
            int okSend = msgsnd(msgQueueReinicio, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al distribuir un mensaje de reinicio a la msgqueue. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
        }
    }

    return 0;
}

