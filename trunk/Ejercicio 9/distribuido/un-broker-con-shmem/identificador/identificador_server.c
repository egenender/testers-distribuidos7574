#include "identificador.h"
#include <stdbool.h>
#include "common/common_c.h"
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
char archivoIpcs[256];

bool inicializado = false;

/* IPC */
int getShMemId(int projId, size_t tamMemory);
void* attachMemory(int shmemId);
int crearSem(int semProjId);
void p(int semaphoreId, int semNum);
void v(int semaphoreId, int semNum);

typedef enum { S2ISUCCESS, S2IOVERFLOW, S2IUNDERFLOW, S2IINCONVERTIBLE } STR2INT_ERROR;

STR2INT_ERROR str2int(int *i, char *s, int base) {
    char *end;
    long  l;
    errno = 0;
    l = strtol(s, &end, base);

    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
        return S2IOVERFLOW;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return S2IUNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return S2IINCONVERTIBLE;
    }
    *i = l;
    return S2ISUCCESS;
}

#define MAX_TAM_LINEA_CONFIG_INI 256
int leerParametroEnteroDeConfigIni( const char* nombreParam ){
    FILE* archivo;
    char linea[MAX_TAM_LINEA_CONFIG_INI];
    char copiaLinea[MAX_TAM_LINEA_CONFIG_INI];
    char* subString;
    int valorParam;

    archivo = fopen ("config.ini", "r");
    if( archivo == NULL )
        return -1;
    while( fgets( linea, MAX_TAM_LINEA_CONFIG_INI, archivo ) ){
        if( linea[0] == '#' )
            continue;
        strcpy(copiaLinea, linea);
        subString = strtok( copiaLinea, "=" );
        if( subString == NULL )
            continue;
        if( strcmp(nombreParam, subString) != 0 )
            continue;
        subString = strtok( copiaLinea, "=" );
        if( subString == NULL )
            continue;
        if( str2int( &valorParam, subString, 10 ) != S2ISUCCESS )
            continue;
        //Read value successfully
        break;
    }
    fclose( archivo );
    return valorParam;
}

int leerParametroStringDeConfigIni( const char* nombreParam, char* valor ){
    FILE* archivo;
    char linea[MAX_TAM_LINEA_CONFIG_INI];
    char copiaLinea[MAX_TAM_LINEA_CONFIG_INI];
    char* subString;

    archivo = fopen ("config.ini", "r");
    if( archivo == NULL )
        return -1;
    while( fgets( linea, MAX_TAM_LINEA_CONFIG_INI, archivo ) ){
        if( linea[0] == '#' )
            continue;
        strcpy(copiaLinea, linea);
        subString = strtok( copiaLinea, "=" );
        if( subString == NULL )
            continue;
        if( strcmp(nombreParam, subString) != 0 )
            continue;
        subString = strtok( copiaLinea, "=" );
        if( subString == NULL )
            continue;
        strcpy( valor, subString );
        break;
    }
    fclose( archivo );
    return 0;
}

void inicializar() {
    char bufferMsjError[255];
    int shmTestersComunes = -1;
    int shmTestersEspeciales = -1;
    int semIdentificador = -1;
    int semTablaTestersComunes = -1;
    int semTablaTestersEspeciales = -1;
    int result = -1;

    //Leer parametros de archivo
    shmTestersComunes = leerParametroEnteroDeConfigIni( "ShmTestersComunesDisponibles" );
    if( shmTestersComunes == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro ShmTestersComunesDisponibles de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    shmTestersEspeciales = leerParametroEnteroDeConfigIni( "ShmTestersEspecialesDisponibles" );
    if( shmTestersEspeciales == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro ShmTestersEspecialesDisponibles de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    semIdentificador = leerParametroEnteroDeConfigIni( "SemIdentificador" );
    if( semIdentificador == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro SemIdentificador de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    semTablaTestersComunes = leerParametroEnteroDeConfigIni( "SemTablaTestersComunesDisponibles" );
    if( semTablaTestersComunes == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro SemTablaTestersComunesDisponibles de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    semTablaTestersEspeciales = leerParametroEnteroDeConfigIni( "SemTablaTestersEspecialesDisponibles" );
    if( semTablaTestersEspeciales == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro SemTablaTestersEspecialesDisponibles de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    result = leerParametroStringDeConfigIni( "ArchivoIpcs", archivoIpcs );
    if( result == -1 ){
        strcpy(bufferMsjError, "Error al leer parametro ArchivoIpcs de config.ini\n");
        write(fileno(stdout), bufferMsjError, strlen(bufferMsjError));
        return;
    }
    //Crear ipcs
    if (shmemTestersComunesDisponiblesId == 0 || testersComunesDisponibles == NULL) {
        shmemTestersComunesDisponiblesId = getShMemId( shmTestersComunes, sizeof(TTablaIdTestersDisponibles) );
        testersComunesDisponibles = (TTablaIdTestersDisponibles*) attachMemory(shmemTestersComunesDisponiblesId);
        for (int i = 0; i < MAX_TESTER_COMUNES; i++)
           testersComunesDisponibles->disponibles[i] = true;
    }
    if (shmemTestersEspecialesDisponiblesId == 0 || testersEspecialesDisponibles == NULL) {
        shmemTestersEspecialesDisponiblesId = getShMemId(shmTestersEspeciales, sizeof(TTablaIdTestersEspecialesDisponibles));
        testersEspecialesDisponibles = (TTablaIdTestersEspecialesDisponibles*) attachMemory(shmemTestersEspecialesDisponiblesId);
        for (int i = 0; i < MAX_TESTER_ESPECIALES; i++)
           testersEspecialesDisponibles->disponibles[i] = true;
    }
    if (semId == 0) {
        semId = crearSem(semIdentificador);
    }
    if (semIdTablaTestersComunes == 0) {
        semIdTablaTestersComunes = crearSem(semTablaTestersComunes);
    }
    if (semIdTablaTestersEspeciales == 0) {
        semIdTablaTestersEspeciales = crearSem(semTablaTestersEspeciales);
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
        }
    }
    v(semIdTablaTestersEspeciales, 0);
    
    if (!exito) {
        result = 0; // No hay espacio para el nuevo tester
    }

    return &result;
}

int *
devolveridtestercomun_1_svc(int *argp, struct svc_req *rqstp)
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
devolveridtesterespecial_1_svc(int *argp, struct svc_req *rqstp)
{
    static int  result;
    
    if (!inicializado)	inicializar();

    int id = *argp;
    if (id >= ID_TESTER_ESP_START + MAX_TESTER_ESPECIALES + 1)  result = -1; // Frula: No existe ese ID
    p(semIdTablaTestersEspeciales, 0);
    if (!testersEspecialesDisponibles->disponibles[id - ID_TESTER_ESP_START]) {
        testersEspecialesDisponibles->disponibles[id - ID_TESTER_ESP_START] = true;
        result = 0;        
    } else {
        result = -1; // Ese ID no estaba registrado, KB!
    }
    v(semIdTablaTestersEspeciales, 0);

    return &result;
}

// IPC

int getShMemId(int projId, size_t tamMemory) {
    char buffer[255];
    key_t key = ftok (archivoIpcs, projId);
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
    key_t key = ftok(archivoIpcs, semProjId);
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

