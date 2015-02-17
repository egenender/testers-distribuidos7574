/* 
 * File:   brokerShMemPassingHandler.cpp
 * Author: ferno
 *
 * Created on February 11, 2015, 12:37 PM
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <climits>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>

#include "logger/Logger.h"
#include "common/common.h"
#include "ipc/Semaphore.h"
#include "sync/Timeout.h"

TMessageShMemInterBroker msg; // Global, para que la maneje tambien el signal handler
bool anilloRestaurado;

void restoreRing(int sigNum) {
    
    key_t key = ftok(ipcFileName.c_str(), SHM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE);
    bool shmListenerEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerEjecutandose = (bool*) shmat(shmListenerEjecutandose, NULL, 0);
    
    Semaphore semListenerEjecutandose(SEM_ANILLO_BROKER_SHM_LISTENER_EJECUTANDOSE);
    semListenerEjecutandose.getSem();
    
    // Si el Listener está corriendo -> ya detectaron la caida del anillo, espero restauracion
    semListenerEjecutandose.p();
    if(*listenerEjecutandose) {
        semListenerEjecutandose.v();
        // Dejo que se termine de ejecutar
    } else {
        semListenerEjecutandose.v();
        // Mato al proceso listener y corro el sender para regenerar el anillo
        key = ftok(ipcFileName.c_str(), SHM_LISTENER_BROKER_SHM_PID);
        int shmListenerBrokerPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
        pid_t* listenerBrokerPid = (pid_t*) shmat(shmListenerBrokerPid, NULL, 0);
        kill(*listenerBrokerPid, SIGINT);
        
        if(fork() == 0) {
            execlp("./anillo/sender", "sender", configBrokerShmemFileName.c_str(), (char*) 0);
            Logger::error("Log luego de execlp del sender para regenerar anillo. Algo salio mal!", __FILE__);
            exit(1);
        }

        sleep(2); // Dejo algo de tiempo para que el sender arranque
    }
    shmdt(listenerEjecutandose);
    
    // Espero hasta que se termine de regenerar el anillo
    Semaphore semAnilloRestaurandose(SEM_ANILLO_BROKER_SHM_RESTAURANDOSE);
    semAnilloRestaurandose.getSem();
    semAnilloRestaurandose.p();

    key = ftok(ipcFileName.c_str(), SHM_BROKER_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);

    Semaphore semSoyLider(SEM_BROKER_ES_LIDER);
    semSoyLider.getSem();

    semSoyLider.p();
    if(*soyLider) {
        semSoyLider.v();
        
        Logger::notice("Me convertí en el líder. Pongo a rodar la shmem inter-broker", __FILE__);
        // Pongo a circular mi memoria compartida mandandomela a mi mismo primero
        key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM);
        int msgQueueRecepcionShmem = msgget(key, IPC_CREAT | 0660);
        int okSend = msgsnd(msgQueueRecepcionShmem, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al reenviarme la Shmem a mi mismo (MASTER) luego de regeneracion del anillo. Abort!", __FILE__);
            exit(1);
        }

    }
    semSoyLider.v();
    shmdt(soyLider);
    
    // Lanzo de vuelta el listener para futuras regeneraciones
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configBrokerShmemFileName.c_str(), (char*) 0);
        Logger::error("Log luego de execlp del listener para seguir escuchando restauraciones. Algo salio mal!", __FILE__);
        exit(1);
    }
    
    anilloRestaurado = true;
    
    Logger::notice("Anillo restaurado!", __FILE__);
}

int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << ID_BROKER;

    // Queue desde donde se reciben mensajes de otros brokers
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM);
	int msgQueueShmemHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM);
	int msgQueueShmemDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_INTERNAL_BROKER_SHM);
	int msgQueueInternaBrokerShmem = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    int shmCantReqBrokerShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* cantReqBrokerShm = (int*) shmat(shmCantReqBrokerShMem, NULL, 0);

    Semaphore semBrokerCantShmemReq(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semBrokerCantShmemReq.getSem();
    
    key = ftok(ipcFileName.c_str(), SHM_BROKER_VERSION);
    int shmIdBrokerVersion = shmget(key, sizeof(unsigned long), 0660);
    unsigned long* idBrokerVersion = (unsigned long*) shmat(shmIdBrokerVersion, NULL, 0);

    Semaphore semIdBrokerVersion(SEM_BROKER_VERSION);
    semIdBrokerVersion.getSem();
    
    key = ftok(ipcFileName.c_str(), SHM_BROKER_SIGUIENTE);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* idBrokerSiguiente = (int*) shmat(shmIdBrokerSiguiente, NULL, 0);

    Semaphore semIdBrokerSiguiente(SEM_BROKER_SIGUIENTE);
    semIdBrokerSiguiente.getSem();
    
    // Determino el handler de la señal en caso de timeout
    struct sigaction action;
    action.sa_handler = restoreRing;
    int sigOk = sigaction(SIGUSR1, &action, 0);
    if (sigOk == -1) {
        Logger::error("Error al setear el handler de la señal!", __FILE__);
        exit(1);
    }

    while(true) {
        
        // Comienza el timeout
        Timeout timeout;
        timeout.runTimeout(SLEEP_TIMEOUT_SHMEM, getpid(), SIGUSR1);
        anilloRestaurado = false;
        // Espero por la llegada de la memoria compartida
        int okRead = msgrcv(msgQueueShmemDesdeBrokers, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0, 0);
        if (okRead == -1) {
            if(anilloRestaurado) {
                // Se ejecuto la restauracion del anillo, intento leer nuevamente
                continue;
            }
            std::stringstream ss;
            ss << "Error al leer de la cola de shared memory desde otros brokers. Errno: " << strerror(errno);
            Logger::error(ss.str(), nombre.str().c_str());
            exit(1);
        }
        timeout.killTimeout();
        
        std::stringstream log;
        log << "Me llego la memoria compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
        /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();
        
        semBrokerCantShmemReq.p();
        int cantRequerimientos = *cantReqBrokerShm;
        semBrokerCantShmemReq.v();
        
        int cantTotalRequerimientos = cantRequerimientos;
        while (cantRequerimientos > 0) {
            log << "Hay " << cantRequerimientos << " modulo(s) del broker " << ID_BROKER << " que necesita(n) la shared memory.";
            Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

            TMessageRequerimientoBrokerShm msgReq;
            okRead = msgrcv(msgQueueInternaBrokerShmem, &msgReq, sizeof(TMessageRequerimientoBrokerShm) - sizeof(long), MTYPE_REQUERIMIENTO_SHM_BROKER, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola interna de gestion de shared memory del broker al buscar requerimiento de shared memory. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            
            log << "El requerimiento obtenido es el del modulo del subbroker " << msgReq.idSubBroker << ". Le envio la memoria...";
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            
            msg.mtype = msgReq.idSubBroker;
            int okSend = msgsnd(msgQueueInternaBrokerShmem, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al enviar shared memory hacia sub-broker " << msg.mtype << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            
            log << "Shared memory enviada al sub-broker " << msg.mtype << ". Espero por su regreso ";
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            
            okRead = msgrcv(msgQueueInternaBrokerShmem, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), MTYPE_DEVOLUCION_SHM_BROKER, 0);
            if (okRead == -1) {
                std::stringstream ss;
                ss << "Error al leer de la cola interna de gestion de shared memory del broker al esperar devolucion de shared memory. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            log << "Shared memory regresa del sub-broker";
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            cantRequerimientos--;
        }
        log << "No hay mas requerimientos de shared memory. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
        Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
        
        // Aumento la version de la shmem ya actualizada
        semIdBrokerVersion.p();
        if(*idBrokerVersion == ULONG_MAX) {
            *idBrokerVersion = 0;
        } else
            *idBrokerVersion += 1;
        semIdBrokerVersion.v();

        // Borro los requerimientos saciados recien
        semBrokerCantShmemReq.p();
        *cantReqBrokerShm = *cantReqBrokerShm - cantTotalRequerimientos;
        semBrokerCantShmemReq.v();
        
        usleep(100000); // sleepeo un toque para no agitar al procesador
        if (CANT_BROKERS == 1) {
            // Soy el unico. Me la envio a mi mismo
            msg.mtype = ID_BROKER;
            int okSend = msgsnd(msgQueueShmemDesdeBrokers, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al enviar shared memory hacia mi. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            } 
        } else {
            // Envio de vuelta la shmem al siguiente broker
            semIdBrokerSiguiente.p();
            msg.mtype = *idBrokerSiguiente;
            semIdBrokerSiguiente.v();
            int okSend = msgsnd(msgQueueShmemHaciaBrokers, &msg, sizeof(TMessageShMemInterBroker) - sizeof(long), 0);
            if (okSend == -1) {
                std::stringstream ss;
                ss << "Error al enviar shared memory hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
        }
    }

    return 0;
}

