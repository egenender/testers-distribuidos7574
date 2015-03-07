#include "DistributedSharedMemory.h"

void* obtenerDistributedSharedMemory(int msgQueueReqShmem, void* requestMsg, size_t sizeMsg, 
        int* cantReqShmem, Semaphore* semCantReqShmem, int msgQueueGetShmem, 
        int sizeMessageSharedMemory, int mtypeGetShm) {
    
    // Primero envío el requerimiento a la Msgqueue y luego aumento 
    // la cantidad de requests
    int okSend = msgsnd(msgQueueReqShmem, requestMsg, sizeMsg - sizeof(long), 0);
    if (okSend == -1) {
        // Si hay algun tipo de error, retorno NULL y cada programa se encargara de exitear
        // mostrando algun error
        return NULL;
    }
    
    semCantReqShmem->p();
    *cantReqShmem += 1;
    semCantReqShmem->v();
    
    // Espero la memoria compartida
    void* distributedSharedMemory = malloc(sizeMessageSharedMemory);
    int okRead = msgrcv(msgQueueGetShmem, distributedSharedMemory, sizeMessageSharedMemory - sizeof(long), mtypeGetShm, 0);
    if (okRead == -1) {
        return NULL;
    }
    return distributedSharedMemory;
}

int devolverDistributedSharedMemory(int msgQueueSendShmem, void* sharedMemory, 
        int sizeMessageSharedMemory) {
    
    int okSend = msgsnd(msgQueueSendShmem, sharedMemory, sizeMessageSharedMemory - sizeof(long), 0);
    if (okSend == -1) {
        return -1;
    }
    return 0;
}
