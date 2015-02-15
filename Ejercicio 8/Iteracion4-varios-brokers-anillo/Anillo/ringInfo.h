/* 
 * File:   ringInfo.h
 * Author: ferno
 *
 * Aca se setea la informacion que el sistema debe usar de los resultados del algoritmo 
 * del anillo. Esto es, quien es el lider, y quien es el broker siguiente
 */

#ifndef RINGINFO_H
#define	RINGINFO_H

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "broker/brokersInfo.h"

void informarLider(bool soyLider) {
    key_t key = ftok("/tmp/pereira-ipcs", SHM_BROKER_ES_LIDER);
    int shmSoyLiderId = shmget(key, sizeof(bool) , 0660);
    bool* lider = (bool*) shmat (shmSoyLiderId, NULL, 0);
    
    Semaphore semSoyLider(SEM_BROKER_ES_LIDER);
    semSoyLider.getSem();
    
    semSoyLider.p();
    *lider = soyLider;
    semSoyLider.v();

    shmdt(lider);
}

void informarBrokerSiguiente(char* ipBrokerSiguiente) {
    key_t key = ftok("/tmp/pereira-ipcs", SHM_BROKER_SIGUIENTE);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int) , 0660);
    int* idBrokerSiguiente = (int*) shmat (shmIdBrokerSiguiente, NULL, 0);

    Semaphore semIdBrokerSiguiente(SEM_BROKER_SIGUIENTE);
    semIdBrokerSiguiente.getSem();
    
    semIdBrokerSiguiente.p();
    *idBrokerSiguiente = 0;
    for (int i = 0; i < CANT_BROKERS && *idBrokerSiguiente == 0; i++){
        if (strcmp(ipBrokerSiguiente, IP_BROKERS[i].ipBroker) == 0){
                *idBrokerSiguiente = IP_BROKERS[i].idBroker;
        }
    }
    semIdBrokerSiguiente.v();

    shmdt(idBrokerSiguiente);
}

#endif	/* RINGINFO_H */

