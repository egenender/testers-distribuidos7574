#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fstream>
#include "commonIdentificador.h"

int main(int argc, char* argv[]) {

	std::fstream ipcFile(ipcFileName.c_str(), std::ios_base::out);
	ipcFile.close();

	// Create and Init Sem
	key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
    int idSem = semget(key, 1, IPC_CREAT | 0666);
	union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *_buf;
    } arg;
    arg.val = 1;
    semctl(idSem, 0, SETVAL, arg);

	pid_t pidServerId = fork();
	if (pidServerId == 0) {
		execlp("./serverId", "serverId", (char*)0);
		std::cout << "Error al forkear y ejecutar el servidor de identificadores!" << std::endl;
		exit(1);
	}

	int status;
	wait(&status);
	return 0;
}
