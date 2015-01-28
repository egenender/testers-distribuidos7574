/* 
 * File:   iniciador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Crea todos los IPCs a usar e inicia todos los procesos correspondientes a la aplicacion
 */

#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"

void createSystemProcesses(int, int, int, int);

int main(int argc, char** argv) {
    if (argc != 5){
		printf("Uso: %s <CANT_DISPOSITIVOS> <MIN_DISP> <MAX_DISP> <TIEMPO_SEP_SIM_MILISEC>\n", argv[0]);
		exit(0);
	}
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    createSystemProcesses(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    
    Logger::destroy();
    
    return 0;
}

void createSystemProcesses(int cant_dispositivos, int min_lanzados, int max_lanzados, int micro_sim) {
   /*if (fork() == 0){
		execlp("./Broker", "Broker", (char*)0);
        Logger::error("Error al ejecutar el programa broker", __FILE__);
        exit(1);
	}
	sleep(3);
   
    // Creo testers
    int idTester = ID_TESTER_START;
    for(int i = 0; i < CANT_TESTERS_COMUNES; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
			// Inicio el programa correspondiente
            execlp("./TesterComun", "TesterComun", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterComun de ID" + idTester, __FILE__);
            exit(1);
        }
    }
    
    for(int i = 0; i < CANT_TESTERS_ESPECIALES; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
			// Inicio el programa correspondiente
            execlp("./TesterEspecial", "TesterEspecial", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterEspecial de ID" + idTester, __FILE__);
            exit(1);
        }
    }

    // Creo al tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./Tecnico", "Tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
        exit(1);
    }
    		
	//Creo dispositivos
    sleep(1);*/
    int idDispositivo = ID_DISPOSITIVO_START;
    int cantidad_lanzada = 0;
    while (cantidad_lanzada < cant_dispositivos){
		int cantidad_a_lanzar = min_lanzados + rand() % (max_lanzados - min_lanzados + 1);
		if (cantidad_a_lanzar + cantidad_lanzada > cant_dispositivos)
			cantidad_a_lanzar = cant_dispositivos - cantidad_lanzada;
		for (int i = 0; i < cantidad_a_lanzar; i++){
			char param[3];
			sprintf(param, "%d", idDispositivo);
			idDispositivo++;
			pid_t newPid = fork();
			if(newPid == 0) {
				// Inicio el programa correspondiente
				execlp("./Dispositivo", "Dispositivo", (char*)0);
				Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
				exit(1);
			}
			usleep(50000);
		}
		cantidad_lanzada += cantidad_a_lanzar;
		long tiempo = micro_sim * 1000;
		usleep(tiempo);
	}

	
    Logger::debug("Programas iniciados correctamente...", __FILE__);
    
}
