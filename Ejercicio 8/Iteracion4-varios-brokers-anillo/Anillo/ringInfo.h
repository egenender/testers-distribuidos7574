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

void informarLider(int shmemId, int semId, int soyLider) {
    key_t key = ftok(ipcFileName.c_str(), shmemId);
    int shmSoyLiderId = shmget(key, sizeof(int) , 0660);
    int* lider = (int*) shmat (shmSoyLiderId, NULL, 0);
    
    if(soyLider) {
        printf("Voy a poner en la shmem que soy el lider");
    } else {
        printf("Voy a poner en la shmem que NO soy el lider");
    }
    
    Semaphore semSoyLider(semId);
    semSoyLider.getSem();
    
    semSoyLider.p();
    *lider = soyLider;
    semSoyLider.v();

    shmdt(lider);
}

void informarBrokerSiguiente(int shmemId, int semId, char* ipBrokerSiguiente) {
    key_t key = ftok(ipcFileName.c_str(), shmemId);
    int shmIdBrokerSiguiente = shmget(key, sizeof(int) , 0660);
    int* idBrokerSiguiente = (int*) shmat (shmIdBrokerSiguiente, NULL, 0);

    Semaphore semIdBrokerSiguiente(semId);
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

unsigned long getVersion(int shmemId, int semId) {

    key_t key = ftok(ipcFileName.c_str(), shmemId);
    int shmIdVersionId = shmget(key, sizeof(unsigned long), 0660);
    unsigned long* version = (unsigned long*) shmat (shmIdVersionId, NULL, 0);

    Semaphore semIdVersion(semId);
    semIdVersion.getSem();
    
    semIdVersion.p();
    unsigned long value = *version;
    shmdt(version);
    semIdVersion.v();
    
    return value;
}

void setListenerEjecutandose(int shmemId, int semId, bool ejecutandose) {

    key_t key = ftok(ipcFileName.c_str(), shmemId);
    int shmIdListenerEjecutandoseId = shmget(key, sizeof(bool) , 0660);
    bool* listenerEjecutandose = (bool*) shmat (shmIdListenerEjecutandoseId, NULL, 0);

    Semaphore semListenerEjecutandose(semId);
    semListenerEjecutandose.getSem();
    
    semListenerEjecutandose.p();
    *listenerEjecutandose = ejecutandose;
    semListenerEjecutandose.v();
    
    shmdt(listenerEjecutandose);
}

void informarAnilloRestaurado(int semAnilloRestaurandoseId) {

    Semaphore semAnilloRestaurandose(semAnilloRestaurandoseId);
    semAnilloRestaurandose.getSem();
    semAnilloRestaurandose.v();

}

#endif	/* RINGINFO_H */

