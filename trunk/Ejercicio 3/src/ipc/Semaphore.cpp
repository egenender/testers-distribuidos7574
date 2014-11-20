#include "Semaphore.h"
#include <sys/sem.h>

using namespace std;

Semaphore::Semaphore( const string& fileName, int identificador) {
    this->identificador = identificador;
    key = ftok(fileName.c_str(),identificador);
}

 // crear un semaforo que no existe
bool Semaphore::creaSem() { 
    semid = semget(key,1, IPC_CREAT | IPC_EXCL | 0660);
    if (semid == -1) return false ;
    existe = true;
    return true;
}
 // adquirir derecho de acceso al semaforo existente
bool Semaphore::getSem() {
    semid = semget(key,1,0660);
    if (semid == -1) return false ;
    existe = true;
    return true;
}

// inicializar el semáforo 
bool Semaphore::iniSem(int val) {
    
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *_buf;
    }arg;
    
    if (!existe) return false;
    arg.val = val;
    return ( semctl(semid, 0, SETVAL, arg) != -1);
}

// liberar el semáforo (v) SIGNAL
bool Semaphore::v() {
    
    struct sembuf oper;
    oper.sem_num = 0;
    oper.sem_op = 1;
    oper.sem_flg = 0;
    if (!existe) return false;
    return(semop(semid,&oper,1) == -1);
}

// ocupar al semáforo (p) WAIT
bool Semaphore::p() {

    struct sembuf oper;
    oper.sem_num = 0;
    oper.sem_op = -1;
    oper.sem_flg = 0;
    if (!existe) return false;
    return(semop(semid,&oper,1) == -1);
}

// eliminar el semáforo del sistema
bool Semaphore::eliSem() {
    return(semctl(semid,0,IPC_RMID) != -1);
}
