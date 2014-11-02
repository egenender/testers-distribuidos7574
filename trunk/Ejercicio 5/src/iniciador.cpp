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

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "common/planilla_local.h"
#include "common/AtendedorDispositivos.h"
#include "common/DespachadorTecnicos.h"

void createIPCObjects();
void createSystemProcesses();

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Inicializando IPCs...", __FILE__);
    
    try {
        createIPCObjects();
    } catch(std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        Logger::destroy();
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);
    
    createSystemProcesses();
    Logger::debug("Procesos iniciados correctamente...", __FILE__);
    
    Logger::notice("Sistema inicializado correctamente...", __FILE__);
    
    Logger::destroy();
    
    return 0;
}

void createIPCObjects() {

    // Creo el archivo que se usara para obtener las keys
    std::fstream ipcFile(ipcFileName.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
	std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();
       
    // Creo semaforo para la shmem de la planilla
    Semaphore semPlanillaGeneral(SEM_PLANILLA_GENERAL);
    semPlanillaGeneral.creaSem();
    semPlanillaGeneral.iniSem(1); // Inicializa el semaforo en 1

    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof(int), IPC_CREAT | 0660);
    int* general = (int*)shmat(shmgeneralid, NULL, 0);
    *general = 0;
    shmdt(general);

    for (int i = 1; i <= CANT_TESTERS; i++){
        Semaphore semtesterA(SEM_TESTER_A + i);
        Semaphore semtesterB(SEM_TESTER_B + i);
        Semaphore semlocal(SEM_PLANILLA_LOCAL + i);
        semtesterA.creaSem();
	    semtesterA.iniSem(1);
	    semtesterB.creaSem();
	    semtesterB.iniSem(0);
	    semlocal.creaSem();
	    semlocal.iniSem(1);

        key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + i);
        int shmlocalid = shmget(key, sizeof(planilla_local_t), IPC_CREAT| 0660);
        //verificacion de errores
        planilla_local_t *shm_planilla_local = (planilla_local_t*)shmat(shmlocalid, NULL, 0);
        shm_planilla_local->cantidad = 0;
        shm_planilla_local->resultados = 0;
        shm_planilla_local->estadoA = LIBRE;
        shm_planilla_local->estadoB = LIBRE;
        shmdt(shm_planilla_local);
        
        key = ftok(ipcFileName.c_str(), 2*(MSGQUEUE_PLANILLA+i));
    }
    
    
    //creacion de colas
    for (int q = MSGQUEUE_ESCRITURA_RESULTADOS; q <= MSGQUEUE_PLANILLA + 1; q++){
		key = ftok(ipcFileName.c_str(), q);
		if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1){
			std::cout << "No se pudo crear una cola: " << strerror(errno)<< std::endl;
		}
	}
          
}

void createSystemProcesses() {
    // Creo dispositivos
    int idDispositivo = ID_DISPOSITIVO_START;
    for(int i = 1; i <= CANT_DISPOSITIVOS; i++) {
        char param[3];
        sprintf(param, "%d", idDispositivo);
        idDispositivo++;
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./Dispositivo", "Dispositivo", param, (char*)0);
            Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
            exit(1);
        }
    }
    
    // Creo testers
    int idTester = ID_TESTER_START;
    for(int i = 0; i < CANT_TESTERS; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        pid_t newPid = fork();
        if(newPid == 0) {
	    pid_t otherPid = fork();
            if (otherPid == 0){
                pid_t planilla = fork();
               	if (planilla == 0){
	            // Inicio el programa correspondiente
                    execlp("./TesterA", "TesterA", param, (char*)0);
                    Logger::error("Error al ejecutar el programa testerA de ID" + idTester, __FILE__);
                    exit(1);
                }else{
                    // Inicio el programa correspondiente
                    execlp("./PlanillaTesterA", "PlanillaTesterA", param, (char*)0);
                    Logger::error("Error al ejecutar el programa PlanillaTesterA de ID" + idTester, __FILE__);
                    exit(1);
                }
            }else{
                pid_t planilla = fork();
                if (planilla == 0){
	            // Inicio el programa correspondiente
                    execlp("./TesterB", "TesterB", param, (char*)0);
                    Logger::error("Error al ejecutar el programa testerB de ID" + idTester, __FILE__);
                    exit(1);
                }else{
                    // Inicio el programa correspondiente
                    execlp("./PlanillaTesterB", "PlanillaTesterB", param, (char*)0);
                    Logger::error("Error al ejecutar el programa PlanillaTesterB de ID" + idTester, __FILE__);
                    exit(1);
                }
			}
        }else{
            pid_t arribos = fork();
            if (arribos == 0){
                 // Inicio el programa correspondiente
                 execlp("./ArriboDeResultados", "ArriboDeResultados", param, (char*)0);
                 Logger::error("Error al ejecutar el programa ArriboDeResultados de ID" + idTester, __FILE__);
                 exit(1);
            }       
        }
    }

    // Creo al tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./Tecnico", "Tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
        exit(1);
    }
    
    Logger::debug("Programas iniciados correctamente...", __FILE__);
    
}
