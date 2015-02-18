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

#include "logger/Logger.h"
#include "common/common.h"
#include "ipc/Semaphore.h"
#include "sync/Timeout.h"

TSharedMemoryPlanillaGeneral msg;
TSharedMemoryPlanillaAsignacion msgPlanillaAsignacion;
bool anilloPlanillaGeneralRestaurado;
bool anilloPlanillaAsignacionRestaurado;

void restoreRingPlanillaGeneral(int sigNum) {
    
    key_t key = ftok(ipcFileName.c_str(), SHM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE);
    bool shmListenerEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerEjecutandose = (bool*) shmat(shmListenerEjecutandose, NULL, 0);
    
    Semaphore semListenerEjecutandose(SEM_ANILLO_PLANILLA_GENERAL_LISTENER_EJECUTANDOSE);
    semListenerEjecutandose.getSem();
    
    // Si el Listener está corriendo -> ya detectaron la caida del anillo, espero restauracion
    semListenerEjecutandose.p();
    if(*listenerEjecutandose) {
        semListenerEjecutandose.v();
        // Dejo que se termine de ejecutar
    } else {
        semListenerEjecutandose.v();
        // Mato al proceso listener y corro el sender para regenerar el anillo
        key = ftok(ipcFileName.c_str(), SHM_LISTENER_PLANILLA_GENERAL_PID);
        int shmListenerPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
        pid_t* listenerPid = (pid_t*) shmat(shmListenerPid, NULL, 0);
        kill(*listenerPid, SIGINT);
        
        if(fork() == 0) {
            execlp("./anillo/sender", "sender", configPlanillaGeneralShmemFileName.c_str(), (char*) 0);
            Logger::error("Log luego de execlp del sender para regenerar anillo. Algo salio mal!", __FILE__);
            exit(1);
        }

        sleep(2); // Dejo algo de tiempo para que el sender arranque
    }
    shmdt(listenerEjecutandose);
    
    // Espero hasta que se termine de regenerar el anillo
    Semaphore semAnilloRestaurandose(SEM_ANILLO_PLANILLA_GENERAL_RESTAURANDOSE);
    semAnilloRestaurandose.getSem();
    semAnilloRestaurandose.p();

    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);

    Semaphore semSoyLider(SEM_PLANILLA_GENERAL_ES_LIDER);
    semSoyLider.getSem();

    semSoyLider.p();
    if(*soyLider) {
        semSoyLider.v();
        
        Logger::notice("Me convertí en el líder. Pongo a rodar la shmem planilla general", __FILE__);
        // Pongo a circular mi memoria compartida mandandomela a mi mismo primero
        key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL);
        int msgQueueRecepcionShmem = msgget(key, IPC_CREAT | 0660);
        int okSend = msgsnd(msgQueueRecepcionShmem, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al reenviarme la Shmem a mi mismo (MASTER) luego de regeneracion del anillo. Abort!", __FILE__);
            exit(1);
        }

    }
    semSoyLider.v();
    shmdt(soyLider);
    
    // Lanzo de vuelta el listener para futuras regeneraciones
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configPlanillaGeneralShmemFileName.c_str(), (char*) 0);
        Logger::error("Log luego de execlp del listener para seguir escuchando restauraciones. Algo salio mal!", __FILE__);
        exit(1);
    }
    
    anilloPlanillaGeneralRestaurado = true;
    
    Logger::notice("Anillo restaurado!", __FILE__);

}

void restoreRingPlanillaAsignacion(int sigNum) {

    key_t key = ftok(ipcFileName.c_str(), SHM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE);
    bool shmListenerEjecutandose = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* listenerEjecutandose = (bool*) shmat(shmListenerEjecutandose, NULL, 0);
    
    Semaphore semListenerEjecutandose(SEM_ANILLO_PLANILLA_ASIGNACION_LISTENER_EJECUTANDOSE);
    semListenerEjecutandose.getSem();
    
    // Si el Listener está corriendo -> ya detectaron la caida del anillo, espero restauracion
    semListenerEjecutandose.p();
    if(*listenerEjecutandose) {
        semListenerEjecutandose.v();
        // Dejo que se termine de ejecutar
    } else {
        semListenerEjecutandose.v();
        // Mato al proceso listener y corro el sender para regenerar el anillo
        key = ftok(ipcFileName.c_str(), SHM_LISTENER_PLANILLA_ASIGNACION_PID);
        int shmListenerPid = shmget(key, sizeof(pid_t), IPC_CREAT | 0660);
        pid_t* listenerPid = (pid_t*) shmat(shmListenerPid, NULL, 0);
        kill(*listenerPid, SIGINT);
        
        if(fork() == 0) {
            execlp("./anillo/sender", "sender", configPlanillaAsignacionShmemFileName.c_str(), (char*) 0);
            Logger::error("Log luego de execlp del sender para regenerar anillo. Algo salio mal!", __FILE__);
            exit(1);
        }

        sleep(2); // Dejo algo de tiempo para que el sender arranque
    }
    shmdt(listenerEjecutandose);
    
    // Espero hasta que se termine de regenerar el anillo
    Semaphore semAnilloRestaurandose(SEM_ANILLO_PLANILLA_ASIGNACION_RESTAURANDOSE);
    semAnilloRestaurandose.getSem();
    semAnilloRestaurandose.p();

    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_ES_LIDER);
    int shmIdSoyLider = shmget(key, sizeof(bool), IPC_CREAT | 0660);
    bool* soyLider = (bool*) shmat(shmIdSoyLider, NULL, 0);

    Semaphore semSoyLider(SEM_PLANILLA_ASIGNACION_ES_LIDER);
    semSoyLider.getSem();

    semSoyLider.p();
    if(*soyLider) {
        semSoyLider.v();
        
        Logger::notice("Me convertí en el líder. Pongo a rodar la shmem planilla asignacion", __FILE__);
        // Pongo a circular mi memoria compartida mandandomela a mi mismo primero
        key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
        int msgQueueRecepcionShmem = msgget(key, IPC_CREAT | 0660);
        int okSend = msgsnd(msgQueueRecepcionShmem, &msgPlanillaAsignacion, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
        if (okSend == -1) {
            Logger::error("Error al reenviarme la Shmem a mi mismo (MASTER) luego de regeneracion del anillo. Abort!", __FILE__);
            exit(1);
        }

    }
    semSoyLider.v();
    shmdt(soyLider);
    
    // Lanzo de vuelta el listener para futuras regeneraciones
    if(fork() == 0) {
        execlp("./anillo/listener", "listener", configPlanillaAsignacionShmemFileName.c_str(), (char*) 0);
        Logger::error("Log luego de execlp del listener para seguir escuchando restauraciones. Algo salio mal!", __FILE__);
        exit(1);
    }
    
    anilloPlanillaAsignacionRestaurado = true;
    
    Logger::notice("Anillo restaurado!", __FILE__);

}

int main(int argc, char** argv) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << ID_BROKER;

    // Queues contra brokers (envio y recepcion de shmems)
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_GENERAL);
	int msgQueuePlanillaGeneralHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_GENERAL);
	int msgQueuePlanillaGeneralDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_BROKER_SHM_PLANILLA_ASIGNACION);
	int msgQueuePlanillaAsignacionHaciaBrokers = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_BROKER_SHM_PLANILLA_ASIGNACION);
	int msgQueuePlanillaAsignacionDesdeBrokers = msgget(key, IPC_CREAT | 0660);

    // Queues contra testers/equipo (envio y recepcion de shmems)
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_SHMEM_HANDLER);
	int msgQueuePlanillasHaciaTesters = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_RECEPCION_SHMEM_HANDLER);
	int msgQueuePlanillasDesdeTesters = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_INTERNAL_REQUERIMIENTO_PLANILLAS_HANDLER);
	int msgQueueCantReqShmemPlanillas = msgget(key, IPC_CREAT | 0660);

    key = ftok(ipcFileName.c_str(), SHM_CANTIDAD_REQUERIMIENTOS_PLANILLAS_SHM);
    int shmCantReqPlanillasShMem = shmget(key, sizeof(int), IPC_CREAT | 0660);
    TShmemCantRequerimientos* cantReqPlanillasShm = (TShmemCantRequerimientos*) shmat(shmCantReqPlanillasShMem, NULL, 0);

    Semaphore semBrokerCantShmemReq(SEM_CANTIDAD_REQUERIMIENTOS_BROKER_SHM);
    semBrokerCantShmemReq.getSem();
    
    // Versiones de las shmem
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_VERSION);
    int shmPlanillaGeneralVersionId = shmget(key, sizeof(unsigned long), IPC_CREAT | 0660);
    unsigned long* planillaGeneralVersion = (unsigned long*) shmat(shmPlanillaGeneralVersionId, NULL, 0);

    Semaphore semPlanillaGeneralVersion(SEM_PLANILLA_GENERAL_VERSION);
    semPlanillaGeneralVersion.getSem();
    
    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_VERSION);
    int shmPlanillaAsignacionVersionId = shmget(key, sizeof(unsigned long), IPC_CREAT | 0660);
    unsigned long* planillaAsignacionVersion = (unsigned long*) shmat(shmPlanillaAsignacionVersionId, NULL, 0);

    Semaphore semPlanillaAsignacionVersion(SEM_PLANILLA_ASIGNACION_VERSION);
    semPlanillaAsignacionVersion.getSem();
    
    if(fork() == 0) {
        // Proceso encargado de la shmem Planilla General

        // Determino el handler de la señal en caso de timeout
        struct sigaction action;
        action.sa_handler = restoreRingPlanillaGeneral;
        int sigOk = sigaction(SIGUSR1, &action, 0);
        if (sigOk == -1) {
            Logger::error("Error al setear el handler de la señal!", __FILE__);
            exit(1);
        }
        
        // Obtengo la shmem del siguiente broker
        key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL_SIGUIENTE);
        int shmIdBrokerSiguientePlanillaGeneral = shmget(key, sizeof(int), 0660);
        int* idBrokerSiguientePlanillaGeneral = (int*) shmat(shmIdBrokerSiguientePlanillaGeneral, NULL, 0);

        Semaphore semIdBrokerSiguientePlanillaGeneral(SEM_PLANILLA_GENERAL_SIGUIENTE);
        semIdBrokerSiguientePlanillaGeneral.getSem();

        while(true) {
            
            // Comienza el timeout
            Timeout timeout;
            timeout.runTimeout(SLEEP_TIMEOUT_SHMEM, getpid(), SIGUSR1);

            anilloPlanillaGeneralRestaurado = false;
            // Espero por la llegada de la memoria compartida desde otros brokers
            int okRead = msgrcv(msgQueuePlanillaGeneralDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0, 0);
            if (okRead == -1) {
                if(anilloPlanillaGeneralRestaurado) {
                    // Se regenero el anillo
                    continue;
                }
                std::stringstream ss;
                ss << "Error al leer de la cola de shared memory de planilla general desde otros brokers. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            
            timeout.killTimeout();

            std::stringstream log;
            log << "Me llego la planilla general compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
            /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();

            semBrokerCantShmemReq.p();
            int cantRequerimientos = cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral;
            semBrokerCantShmemReq.v();

            int cantTotalRequerimientos = cantRequerimientos;
            while (cantRequerimientos > 0) {
                log << "Hay " << cantRequerimientos << " testers/equipo comunicados con broker " << ID_BROKER << " que necesita(n) la planilla general.";
                Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                TRequerimientoSharedMemory msgReq;
                okRead = msgrcv(msgQueueCantReqShmemPlanillas, &msgReq, sizeof(TRequerimientoSharedMemory) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_GENERAL, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola interna de requerimientos de planillas. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "El requerimiento obtenido es el del tester/equipo " << msgReq.idSolicitante << ". Le envio la planilla general...";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                msg.mtype = msgReq.idDevolucion;
                int okSend = msgsnd(msgQueuePlanillasHaciaTesters, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia tester/equipo " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "Planilla general enviada al tester/equipo " << msg.mtype << ". Espero por su regreso ";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                okRead = msgrcv(msgQueuePlanillasDesdeTesters, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola de devolucion de planilla general. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                log << "Planilla general regresa";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
                cantRequerimientos--;
            }
            log << "No hay mas requerimientos de planilla general. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
            
            // Aumento la version de la shmem
            semPlanillaGeneralVersion.p();
            if(*planillaGeneralVersion == ULONG_MAX) {
                *planillaGeneralVersion = 0;
            } else
                *planillaGeneralVersion += 1;
            semPlanillaGeneralVersion.v();

            // Borro los requerimientos saciados recien
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral = cantReqPlanillasShm->cantRequerimientosShmemPlanillaGeneral - cantTotalRequerimientos;
            semBrokerCantShmemReq.v();

            // Pequeño sleep para no matar al CPU si hay pocos brokers
            if (CANT_BROKERS <= 3) {
                sleep(3 - CANT_BROKERS);
            } else {
                usleep(10000);
            }

            if (CANT_BROKERS == 1) {
                // Soy el unico. Me la envio a mi mismo
                msg.mtype = ID_BROKER;
                int okSend = msgsnd(msgQueuePlanillaGeneralDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia mi. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                } 
            } else {
                // Envio de vuelta la shmem al siguiente broker
                semIdBrokerSiguientePlanillaGeneral.p();
                msg.mtype = *idBrokerSiguientePlanillaGeneral;
                semIdBrokerSiguientePlanillaGeneral.v();
                int okSend = msgsnd(msgQueuePlanillaGeneralHaciaBrokers, &msg, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla general hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
            }
        }
        shmdt((void*) idBrokerSiguientePlanillaGeneral);
    }
    
    if(fork() == 0) {
        // Proceso encargado de la shmem Planilla Asignacion
        
        // Determino el handler de la señal en caso de timeout
        struct sigaction action;
        action.sa_handler = restoreRingPlanillaAsignacion;
        int sigOk = sigaction(SIGUSR1, &action, 0);
        if (sigOk == -1) {
            Logger::error("Error al setear el handler de la señal!", __FILE__);
            exit(1);
        }
        
        key = ftok(ipcFileName.c_str(), SHM_PLANILLA_ASIGNACION_SIGUIENTE);
        int shmIdBrokerSiguientePlanillaAsignacion = shmget(key, sizeof(int), IPC_CREAT | 0660);
        int* idBrokerSiguientePlanillaAsignacion = (int*) shmat(shmIdBrokerSiguientePlanillaAsignacion, NULL, 0);

        Semaphore semIdBrokerSiguientePlanillaAsignacion(SEM_PLANILLA_ASIGNACION_SIGUIENTE);
        semIdBrokerSiguientePlanillaAsignacion.getSem();

        while(true) {

            // Comienza el timeout
            Timeout timeout;
            timeout.runTimeout(SLEEP_TIMEOUT_SHMEM, getpid(), SIGUSR1);

            anilloPlanillaGeneralRestaurado = false;
            // Espero por la llegada de la memoria compartida desde otros brokers
            int okRead = msgrcv(msgQueuePlanillaAsignacionDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0, 0);
            if (okRead == -1) {
                if(anilloPlanillaGeneralRestaurado) {
                    // Se estaba regenerando el anillo intento leer nuevamente
                    continue;
                }
                std::stringstream ss;
                ss << "Error al leer de la cola de planilla asignacion desde otros brokers. Errno: " << strerror(errno);
                Logger::error(ss.str(), nombre.str().c_str());
                exit(1);
            }
            timeout.killTimeout();

            std::stringstream log;
            log << "Me llego la planilla asignacion compartida. Broker ID: " << ID_BROKER << ". Me fijo si alguien la necesita.";
            /*Logger::debug(log.str(), nombre.str().c_str());*/ log.str(""); log.clear();

            semBrokerCantShmemReq.p();
            int cantRequerimientos = cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion;
            semBrokerCantShmemReq.v();

            int cantTotalRequerimientos = cantRequerimientos;
            while (cantRequerimientos > 0) {
                log << "Hay " << cantRequerimientos << " testers/equipo comunicados con broker " << ID_BROKER << " que necesita(n) la planilla asignacion.";
                Logger::notice(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                TRequerimientoSharedMemory msgReq;
                okRead = msgrcv(msgQueueCantReqShmemPlanillas, &msgReq, sizeof(TRequerimientoSharedMemory) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola interna de requerimientos de planillas. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "El requerimiento obtenido es el del tester/equipo " << msgReq.idSolicitante << ". Le envio la planilla asignacion...";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                msg.mtype = msgReq.idSolicitante;
                int okSend = msgsnd(msgQueuePlanillasHaciaTesters, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia tester/equipo " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }

                log << "Planilla asignacion enviada al tester/equipo " << msg.mtype << ". Espero por su regreso ";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

                okRead = msgrcv(msgQueuePlanillasDesdeTesters, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION, 0);
                if (okRead == -1) {
                    std::stringstream ss;
                    ss << "Error al leer de la cola de devolucion de planilla asignacion. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
                log << "Planilla asignacion regresa";
                Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();
                cantRequerimientos--;
            }
            log << "No hay mas requerimientos de planilla asignacion. Se la mando al siguiente broker de ID " << ID_BROKER_SIGUIENTE;
            Logger::debug(log.str(), nombre.str().c_str()); log.str(""); log.clear();

            // Aumento la version de la shmem
            semPlanillaAsignacionVersion.p();
            if(*planillaAsignacionVersion == ULONG_MAX) {
                *planillaAsignacionVersion = 0;
            } else
                *planillaAsignacionVersion += 1;
            semPlanillaAsignacionVersion.v();

            // Borro los requerimientos saciados recien
            semBrokerCantShmemReq.p();
            cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion = cantReqPlanillasShm->cantRequerimientosShmemPlanillaAsignacion - cantTotalRequerimientos;
            semBrokerCantShmemReq.v();

            usleep(10000); // sleepeo un toque para no agitar al procesador
            if (CANT_BROKERS == 1) {
                // Soy el unico. Me la envio a mi mismo
                msg.mtype = ID_BROKER;
                int okSend = msgsnd(msgQueuePlanillaAsignacionDesdeBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia mi. Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                } 
            } else {
                // Envio de vuelta la shmem al siguiente broker
                semIdBrokerSiguientePlanillaAsignacion.p();
                msg.mtype = *idBrokerSiguientePlanillaAsignacion;
                semIdBrokerSiguientePlanillaAsignacion.v();
                int okSend = msgsnd(msgQueuePlanillaAsignacionHaciaBrokers, &msg, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
                if (okSend == -1) {
                    std::stringstream ss;
                    ss << "Error al enviar planilla asignacion hacia siguiente broker " << msg.mtype << ". Errno: " << strerror(errno);
                    Logger::error(ss.str(), nombre.str().c_str());
                    exit(1);
                }
            }
        }
    }

    return 0;
}

