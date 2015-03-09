/* 
 * File:   DistribuidorMensajesTestersComunes.cpp
 * Author: knoppix
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
#include "common/Configuracion.h"

using namespace Constantes::NombresDeParametros;
using namespace std;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );
    
    std::fstream ipcFile(archivoIpcs.c_str(), std::ios_base::out);
    ipcFile.close();
    
    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_TESTER_COMUN) );
    int msgQueueTodos = msgget(key, 0666);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN) );
    int msgQueueReq = msgget(key, 0666);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN) );
    int msgQueueGral = msgget(key, 0666);
    
    while(true) {
    
        TMessageAtendedor msg;
        int okRead = msgrcv(msgQueueTodos, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
        if (okRead == -1) {
            std::stringstream ss;
            ss << "Error al leer un mensaje para distribuir. Errno: " << strerror(errno);
            Logger::error(ss.str(), __FILE__);
            exit(1);
        }
        
        // Si llega un requerimiento lo envio a la cola de requerimientos, sino a la cola general
        if(msg.mtypeMensaje == Constantes::Mtypes::MTYPE_REQUERIMIENTO_DISPOSITIVO) {
            std::stringstream log;
            log << "Distribuyo mensaje de requerimiento del dispositivo " << msg.idDispositivo << " para el tester " << msg.mtype;
            Logger::notice(log.str(), __FILE__); log.str(""); log.clear();
            
            int okSend = msgsnd(msgQueueReq, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al distribuir un requerimiento a la msgqueue. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
        } else {
            std::stringstream log;
            log << "Distribuyo mensaje general del dispositivo " << msg.idDispositivo << " para el tester " << msg.mtype;
            Logger::notice(log.str(), __FILE__); log.str(""); log.clear();
            
            int okSend = msgsnd(msgQueueGral, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al distribuir un mensaje general a la msgqueue. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
        }
    }

    return 0;
}

