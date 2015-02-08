#include "identificador.h"
#include <stdbool.h>
#include "common/common.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semId = 0; // Representa el ID del semaforo que lockea las variables globales de identificador
int semIdTablaTestersComunes = 0;
int semIdTablaTestersEspeciales = 0;
int shmemTestersComunesDisponiblesId = 0;
int shmemTestersEspecialesDisponiblesId = 0;
TTablaIdTestersDisponibles* testersComunesDisponibles = NULL;
TTablaIdTestersEspecialesDisponibles* testersEspecialesDisponibles = NULL;

bool inicializado = false;

/* IPC */
int getShMemId(int projId, size_t tamMemory);
void* attachMemory(int shmemId);
int crearSem(int semProjId);
void p(int semaphoreId, int semNum);
void v(int semaphoreId, int semNum);

void inicializar() {
	
    if (shmemTestersComunesDisponiblesId == 0 || testersComunesDisponibles == NULL) {
        shmemTestersComunesDisponiblesId = getShMemId(SHM_TESTERS_COMUNES_DISPONIBLES, sizeof(TTablaIdTestersDisponibles));
        testersComunesDisponibles = (TTablaIdTestersDisponibles*) attachMemory(shmemTestersComunesDisponiblesId);
        for (int i = 0; i < MAX_TESTER_COMUNES; i++)   testersComunesDisponibles->disponibles[i] = true;
        testersComunesDisponibles->ultimoTesterElegido = 0;
    }
    if (shmemTestersEspecialesDisponiblesId == 0 || testersEspecialesDisponibles == NULL) {
        shmemTestersEspecialesDisponiblesId = getShMemId(SHM_TESTERS_ESPECIALES_DISPONIBLES, sizeof(TTablaIdTestersEspecialesDisponibles));
        testersEspecialesDisponibles = (TTablaIdTestersEspecialesDisponibles*) attachMemory(shmemTestersEspecialesDisponiblesId);
        for (int i = 0; i < MAX_TESTER_ESPECIALES; i++)   testersEspecialesDisponibles->disponibles[i] = true;
        testersEspecialesDisponibles->ultimoTesterElegido = 0;
    }
    if (semId == 0) {
        semId = crearSem(SEM_IDENTIFICADOR);
    }
    if (semIdTablaTestersComunes == 0) {
        semIdTablaTestersComunes = crearSem(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES);
    }
    if (semIdTablaTestersEspeciales == 0) {
        semIdTablaTestersEspeciales = crearSem(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES);
    }
    inicializado = true;
}

int *
getiddispositivo_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;

	if (!inicializado)	inicializar();

	p(semId, 0);
	result = ID_DISPOSITIVO++;
	v(semId, 0);

	return &result;
}

int *
getidtestercomun_1_svc(void *argp, struct svc_req *rqstp)
{
    static int  result;

    if (!inicializado)	inicializar();
    
    bool exito = false;
    p(semIdTablaTestersComunes, 0);
    for (int i = 0; (i < MAX_TESTER_COMUNES) && (!exito); i++) {
        if (testersComunesDisponibles->disponibles[i]) {
            result = i + ID_TESTER_COMUN_START; // Para que funcione como mtype
            testersComunesDisponibles->disponibles[i] = false;
            exito = true;
        }
    }
    v(semIdTablaTestersComunes, 0);
    
    if (!exito) {
        result = 0; // No hay espacio para el nuevo tester
    }

    return &result;
}

int *
getidtesterespecial_1_svc(void *argp, struct svc_req *rqstp)
{
    static int  result;

    if (!inicializado)	inicializar();

    bool exito = false;
    p(semIdTablaTestersEspeciales, 0);
    for (int i = 0; (i < MAX_TESTER_ESPECIALES) && (!exito); i++) {
        if (testersEspecialesDisponibles->disponibles[i]) {
            result = i + ID_TESTER_ESP_START; // Para que funcione como mtype
            testersEspecialesDisponibles->disponibles[i] = false;
            exito = true;
            
            // Levanto el semaforo por si hay un requerimiento especial esperando
            key_t key = ftok(ipcFileName.c_str(), SEM_ESPECIALES + result);
            int semEspecial = semget(key, 1, 0660);
            struct sembuf oper;
            oper.sem_num = 0;
            oper.sem_op = 1;
            oper.sem_flg = 0;
            semop(semEspecial, &oper, 1);
        }
    }
    v(semIdTablaTestersEspeciales, 0);
    
    if (!exito) {
        result = 0; // No hay espacio para el nuevo tester
    }

    return &result;
}

int *
desregistrartestercomun_1_svc(int *argp, struct svc_req *rqstp)
{
    static int  result;
    
    if (!inicializado)	inicializar();

    int id = *argp;
    if (id >= ID_TESTER_COMUN_START + MAX_TESTER_COMUNES + 1)  result = -1; // Frula: No existe ese ID
    p(semIdTablaTestersComunes, 0);
    if (!testersComunesDisponibles->disponibles[id - ID_TESTER_COMUN_START]) {
        testersComunesDisponibles->disponibles[id - ID_TESTER_COMUN_START] = true;
        result = 0;
    } else {
        result = -1; // Ese ID no estaba registrado, KB!
    }
    v(semIdTablaTestersComunes, 0);

    return &result;
}

int *
desregistrartesterespecial_1_svc(int *argp, struct svc_req *rqstp)
{
    static int  result;
    
    if (!inicializado)	inicializar();

    int id = *argp;
    if (id >= ID_TESTER_ESP_START + MAX_TESTER_ESPECIALES + 1)  result = -1; // Frula: No existe ese ID
    p(semIdTablaTestersEspeciales, 0);
    if (!testersEspecialesDisponibles->disponibles[id - ID_TESTER_ESP_START]) {
        testersEspecialesDisponibles->disponibles[id - ID_TESTER_ESP_START] = true;
        result = 0;
        
        // Bajo el semaforo que indica que el tester esta disponible
        key_t key = ftok(ipcFileName.c_str(),SEM_ESPECIALES + id - ID_TESTER_ESP_START);
        int semEspecial = semget(key, 1, 0660);
        struct sembuf oper;
        oper.sem_num = 0;
        oper.sem_op = -1;
        oper.sem_flg = 0;
        semop(semEspecial, &oper, 1);
        
    } else {
        result = -1; // Ese ID no estaba registrado, KB!
    }
    v(semIdTablaTestersEspeciales, 0);

    return &result;
}

// IPC

int getShMemId(int projId, size_t tamMemory) {
    char buffer[255];
    key_t key = ftok (ipcFileName.c_str(), projId);
    if ( key == -1 ) {
            strcpy(buffer, "Error en SharedMemory - ftok:");
            strcat(buffer, strerror(errno));        
            strcat(buffer, "\n");
            write(fileno(stdout), buffer, strlen(buffer)); 
            return -1;
    }

    int id = shmget(key, tamMemory , IPC_CREAT | 0666);
    if ( id == -1 ) {
            strcpy(buffer, "Error en SharedMemory - shmget:");
            strcat(buffer, strerror(errno));
            strcat(buffer, "\n");
            write(fileno(stdout), buffer, strlen(buffer)); 
            return -1;
    }
    
    return id;
}

void* attachMemory(int shmemId) {
        /* Me attacho a la memoria dejando al SO que elija donde ubicar la memoria 
         * (atributo en NULL)y para lectura/escritura (atributo en 0) */
        char buffer[255];
        void *shmaddr = shmat(shmemId, (void *)NULL, 0);
        if (shmaddr == (void *)-1) {
                strcpy(buffer, "Error en Shared Memory - shmat:");
                strcat(buffer, strerror(errno));
                strcat(buffer, "\n");   
                write(fileno(stdout), buffer, strlen(buffer)); 
                return NULL;
        }
        return shmaddr;
}

int crearSem(int semProjId) {

	key_t key = ftok(ipcFileName.c_str(), semProjId);
	return (semget(key, 1, 0666));
}

void p(int semaphoreId, int semNum) {
    struct sembuf oper;
    oper.sem_num = semNum;
    oper.sem_op = -1;
    semop(semaphoreId, &oper, 1);
}

void v(int semaphoreId, int semNum) {
    struct sembuf oper;
    oper.sem_num = semNum;
    oper.sem_op = 1;
    oper.sem_flg = 0;
    semop(semaphoreId, &oper, 1);
}

