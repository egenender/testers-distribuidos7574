#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"

int main (void) {
    
	Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_REQUERIMIENTOS_DISPOSITIVOS);
	int msgQueueReqDisp = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR);
	int msgQueueTesterComun = msgget(key, 0660);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_EMISOR_DISPOSITIVOS);
	int msgQueueADisp = msgget(key, 0660);
/*
	Semaphore sem_comunes(SEM_CANT_TESTERS_COMUNES);
	sem_comunes.getSem();
*/
	key = ftok(ipcFileName.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTablaTestersComunesDisp = shmget(key, sizeof(TTablaIdTestersDisponibles), IPC_CREAT | 0660);
    TTablaIdTestersDisponibles* tablaTestersComunesDisp = (TTablaIdTestersDisponibles*) shmat(shmTablaTestersComunesDisp, (void*) NULL, 0);
    
    Semaphore semTabla(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    semTabla.getSem();

	TMessageAtendedor msg;
	while(true) {
		Logger::notice("Espero por un requerimiento de dispositivo...", __FILE__);
		int okRead = msgrcv(msgQueueReqDisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
		if (okRead == -1){
			exit(0);
		}

		std::stringstream ss;
		ss << "Me llego un requerimiento desde el dispositivo " << msg.idDispositivo;
		Logger::notice(ss.str(), __FILE__);

/* TODO: IMP!		Logger::notice("Obtengo semaforo para actuar, esperando que haya testers comunes", __FILE__);
		sem_comunes.p();
		Logger::notice("Ya hay un tester comun, entonces puedo enviar el requerimiento", __FILE__);
*/
        bool exito = true;
		semTabla.p();
        // Busco un id que no este disponible (es decir, uno registrado)
        for(int i = tablaTestersComunesDisp->ultimoTesterElegido + 1; i <= MAX_TESTER_COMUNES; i++) {
            if(i == MAX_TESTER_COMUNES) i = 0;
            if(!tablaTestersComunesDisp->disponibles[i]) {
                msg.mtype = i + ID_TESTER_COMUN_START;
                tablaTestersComunesDisp->ultimoTesterElegido = i;
                break;
            }
            if (i == tablaTestersComunesDisp->ultimoTesterElegido) {
                std::stringstream ss;
                ss << "No hay tester comun disponible para el dispositivo " << msg.idDispositivo;
                Logger::error(ss.str(), __FILE__);
                exito = false;
                break;
            }
        }
		semTabla.v();
        
        ss.str("");
        ss.clear();
        // ¿Puede ser que no haya testers comunes registrados? En ese caso
        // le envio msg al dispositivo con el "SIN_LUGAR"
        if (!exito) {
            ss << "Le envio que no hay lugar al dispositivo " << msg.idDispositivo;
            Logger::notice(ss.str(), __FILE__);
            
            msg.mtype = msg.idDispositivo;
            msg.value = SIN_LUGAR;
            int ret = msgsnd(msgQueueADisp, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if(ret == -1) {
                Logger::error("Error al enviar el SIN_LUGAR al dispositivo", __FILE__);
                exit(1);
            }
        } else {
            ss << "Le envio el requerimiento del dispositivo al Tester " << msg.mtype;
            Logger::notice(ss.str(), __FILE__);

            int ret = msgsnd(msgQueueTesterComun, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if(ret == -1) {
                Logger::error("Error al enviar el requerimiento del dispositivo al tester", __FILE__);
                exit(1);
            }
        }
	}
}