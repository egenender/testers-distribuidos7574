#include <cstdio>
#include "commonIdentificador.h"

int main(int argc, char* argv[]) {

	// Elimino el semaforo
	key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
    int idSem = semget(key, 1, IPC_CREAT | 0666);
	semctl(idSem, 0, IPC_RMID, (struct semid_ds*)0);

	// Borro el archivo de IPC
	remove(ipcFileName.c_str());
}
