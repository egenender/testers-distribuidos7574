#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "common/common.h"

class Semaphore {
public:
    Semaphore(int);
    bool iniSem(int);
    bool getSem();
    bool creaSem();
    bool p();
    bool v();
    bool eliSem();

private:
    int identificador; // nro de IPC del mismo directorio p/clave
    int semid; // el handler del semaforo
    key_t key; // la clave para obtener el semaforo
    bool existe; // si ha sido creado
};
