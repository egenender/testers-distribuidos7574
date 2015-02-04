#include "identificador.h"
#include "stdbool.h"
#include "commonIdentificador.h"

int semId = 0; // Representa el ID del semaforo que lockea las variables globales de identificador
int shmemTestersComunesDisponiblesId = 0;
int shmemTestersEspecialesDisponiblesId = 0;
bool* testersComunesDisponibles = NULL;
bool* testersEspecialesDisponibles = NULL;

bool inicializado = false;

/* IPC */
int getSharedMemory(char *fileName, int id);
int getShMemId(char *fileName, int id, int flags);
int attachMemory();
int crearSem();
void p(int semId);
void v(int semId);

void inicializar() {
	
    if (shmemTestersComunesDisponiblesId == 0 || testersComunesDisponibles == NULL) {
        shmemTestersComunesDisponiblesId = getShMemId(SHM_TESTERS_COMUNES_DISPONIBLES, sizeof(bool) * MAX_TESTERS_COMUNES);
        testersComunesDisponibles = (bool*) attachMemory(shmemTestersComunesDisponiblesId);
        for (int i = 0; i < MAX_TESTERS_COMUNES; i++)   testersComunesDisponibles[i] = true;
    }
    if (shmemTestersEspecialesDisponiblesId == 0 || testersEspecialesDisponibles == NULL) {
        shmemTestersEspecialesDisponiblesId = getShMemId(SHM_TESTERS_ESPECIALES_DISPONIBLES, sizeof(bool) * MAX_TESTERS_ESPECIALES);
        testersEspecialesDisponibles = (bool*) attachMemory(shmemTestersEspecialesDisponiblesId);
        for (int i = 0; i < MAX_TESTERS_ESPECIALES; i++)   testersEspecialesDisponibles[i] = true;
    }
    if (semId == 0) {
            semId = crearSem();
    }
    inicializado = true;
}

int *
getiddispositivo_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;

	if (!inicializado)	inicializar();

	p(0);
	result = ID_DISPOSITIVO++;
	v(0);

	return &result;
}

int *
getidtestercomun_1_svc(void *argp, struct svc_req *rqstp)
{
    static int  result;

    if (!inicializado)	inicializar();
    
    bool exito = false;
    p(0);
    for (int i = 0; (i < MAX_TESTERS_COMUNES) && (!exito); i++) {
        if (testersComunesDisponibles[i]) {
            result = i + 1; // Para que funcione como mtype
            testersComunesDisponibles[i] = false;
            exito = true;
        }
    }
    v(0);
    
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
    p(0);
    for (int i = 0; (i < MAX_TESTERS_ESPECIALES) && (!exito); i++) {
        if (testersEspecialesDisponibles[i]) {
            result = i + 1; // Para que funcione como mtype
            testersEspecialesDisponibles[i] = false;
            exito = true;
        }
    }
    v(0);
    
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
    if (id >= MAX_TESTER_COMUNES + 1)  result = -1; // Frula: No existe ese ID
    p(0);
    if (!testersComunesDisponibles[id - 1]) {
        testersComunesDisponibles[id - 1] = true;
        result = 0;
    } else {
        result = -1; // Ese ID no estaba registrado, KB!
    }
    v(0);

    return &result;
}

int *
desregistrartesterespecial_1_svc(int *argp, struct svc_req *rqstp)
{
    static int  result;
    
    if (!inicializado)	inicializar();

    int id = *argp;
    if (id >= MAX_TESTER_ESPECIALES + 1)  result = -1; // Frula: No existe ese ID
    p(0);
    if (!testersEspecialesDisponibles[id - 1]) {
        testersEspecialesDisponibles[id - 1] = true;
        result = 0;
    } else {
        result = -1; // Ese ID no estaba registrado, KB!
    }
    v(0);

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

    int id = shmget( clave, tamMemory , IPC_CREAT | 0666);
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
                return -1;
        }
        return shmaddr;
}

int crearSem() {

	key_t key = ftok(ipcFileName.c_str(), SEM_IDENTIFICADOR);
	return (semget(key, 1, IPC_CREAT | 0666));
}

void p(int semNum) {
	struct sembuf oper;
    oper.sem_num = semNum;
    oper.sem_op = -1;
    semop(semId, &oper, 1);
}

void v(int semNum) {
	struct sembuf oper;
    oper.sem_num = semNum;
    oper.sem_op = 1;
    oper.sem_flg = 0;
    semop(semId, &oper, 1);
}
