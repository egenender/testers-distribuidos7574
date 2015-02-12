/* 
 * File:   terminadorDisp.cpp
 * Author: ferno
 *
 * Created on February 7, 2015, 10:02 PM
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "common/common.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    // Destruyo IPCs
    for (int q = MSGQUEUE_ENVIO_DISP; q <= MSGQUEUE_RECEPCIONES_DISP; q++){
        key_t key = ftok(ipcFileName.c_str(), q);
        int queue = msgget(key, 0660);
        if (queue == -1) {
            std::cout << "No se pudo obtener una cola: " << strerror(errno) << std::endl;
        }
        msgctl(queue ,IPC_RMID, NULL);
    }
    
//    unlink(ipcFileName.c_str());

    return 0;
}

