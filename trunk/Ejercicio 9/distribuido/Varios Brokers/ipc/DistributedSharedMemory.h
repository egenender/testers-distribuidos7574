/* 
 * File:   DistributedSharedMemory.h
 * Author: ferno
 *
 * Created on February 11, 2015, 2:19 PM
 */

#ifndef DISTRIBUTEDSHAREDMEMORY_H
#define	DISTRIBUTEDSHAREDMEMORY_H

#include <sys/shm.h>
#include <sys/msg.h>
#include <cstdlib>
#include "Semaphore.h"

void* obtenerDistributedSharedMemory(int msgQueueReqShmem, void* msg, size_t sizeMsg, 
        int* cantReqShmem, Semaphore* semCantReqShmem, int msgQueueGetShmem, 
        int sizeMessageSharedMemory, int mtypeGetShm);
int devolverDistributedSharedMemory(int msgQueueSendShmem, void* sharedMemory, int sizeSharedMemory);

#endif	/* DISTRIBUTEDSHAREDMEMORY_H */

