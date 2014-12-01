#pragma once

#include <sys/ipc.h>

#include <string>

class Semaphore {
public:
    Semaphore(const std::string& fileName,int);
    bool iniSem(int);
    bool getSem();
    bool creaSem();
    bool p();
    bool v();
    bool eliSem();
    int getSemId();

private:
    int identificador; // nro de IPC del mismo directorio p/clave
    int semid; // el handler del semaforo
    key_t key; // la clave para obtener el semaforo
    bool existe; // si ha sido creado
};

