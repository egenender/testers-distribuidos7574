/* 
 * File:   iniciador.cpp
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:09 PM
 * 
 * Crea todos los IPCs a usar e inicia todos los procesos correspondientes a la aplicacion
 */

#include "common/common.h"
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "common/Configuracion.h"
#include "common/Planilla.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>

using namespace Constantes::NombresDeParametros;

void crearObjetosIPC( const Configuracion& config );
void lanzarProcesosSistema( const Configuracion& config );

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Inicializando IPCs...", __FILE__);

    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }

    try {
        crearObjetosIPC( config );
    } catch(std::string err) {
        Logger::error("Error al crear los objetos activos...", __FILE__);
        Logger::destroy();
        return 1;
    }
    Logger::debug("Objetos IPC inicializados correctamente. Iniciando procesos...", __FILE__);
    
    lanzarProcesosSistema( config );
    Logger::debug("Procesos iniciados correctamente...", __FILE__);
    
    Logger::notice("Sistema inicializado correctamente...", __FILE__);
    
    Logger::destroy();
    
    return 0;
}

void CrearSemaforo( const std::string& archivoIpcs, int idSemaforo, int valorInicial ){
    Semaphore sem( archivoIpcs, idSemaforo );
    sem.creaSem();
    sem.iniSem( valorInicial );
}

void crearObjetosIPC( const Configuracion& config ) {
    const std::string archivoIpcs = config.ObtenerParametroString(
        Constantes::NombresDeParametros::ARCHIVO_IPCS.c_str() );

    // Creo el archivo que se usara para obtener las keys
    std::fstream ipcFile(archivoIpcs.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
    std::string err = std::string("Error creando el archivo de IPCs. Error: ")
        + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();
    
    CrearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL), 1 );
    CrearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_COLA_ESPECIALES), 1 );
    CrearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS), 1 );
    CrearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS), 1 );
    


    Planilla planillaGeneral( config );
    planillaGeneral.initPlanilla();
    PlanillaAsignacionEquipoEspecial planillaAsignacion( config );
    planillaAsignacion.initPlanilla();
    
    //Semaforos planilla variables config
    for( int i=0; i<config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA ); i++ ){
        Semaphore semPlanillaVarsTE( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_TE_START) + i );
        semPlanillaVarsTE.creaSem();
        semPlanillaVarsTE.iniSem(1);
        Semaphore semPlanillaVarsCV( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_CV_START) + i );
        semPlanillaVarsCV.creaSem();
        semPlanillaVarsCV.iniSem(1);        
        Semaphore semMutexPlanillaVars( archivoIpcs, config.ObtenerParametroEntero(SEM_MUTEX_PLANILLA_VARS_START) + i );
        semMutexPlanillaVars.creaSem();
        semMutexPlanillaVars.iniSem(1);
    }
    
    //Creacion de colas (una para cada dispositivo)
    int msgQueueDispositivos = config.ObtenerParametroEntero( MSGQUEUE_DISPOSITIVOS );
    int msgQueueUltimo = config.ObtenerParametroEntero( MSGQUEUE_ULTIMO );
    for (int q = msgQueueDispositivos; q <= msgQueueUltimo; q++){
        key_t key = ftok(archivoIpcs.c_str(), q);
        if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1){
            std::cout << "No se pudo crear una cola: " << strerror(errno)<< std::endl;
        }
    }
    //Creacion de cola-config (una para todos los dispositivos, reciben por mtype + id)
    int msgQueueDispositivosConfig = config.ObtenerParametroEntero( MSGQUEUE_DISPOSITIVOS_CONFIG );
    key_t key = ftok(archivoIpcs.c_str(), msgQueueDispositivosConfig);
    if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1){
        std::cout << "No se pudo crear la cola de dispositivos-config: " << strerror(errno)<< std::endl;
    }
    //Creacion de Cola testers config
    int msgQueueTestersConfig = config.ObtenerParametroEntero( MSGQUEUE_TESTERS_CONFIG );
    key = ftok(archivoIpcs.c_str(), msgQueueTestersConfig);
    if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1){
        std::cout << "No se pudo crear la cola de testers-config: " << strerror(errno)<< std::endl;
    }
    //Shared memories dispositivo-config
    
}

void lanzarProcesosSistema( const Configuracion& config ) {
   
    // Creo testers
    const int cantTestersComunes = config.ObtenerParametroEntero( CANT_TESTERS_COMUNES );
    int idTester = config.ObtenerParametroEntero( ID_TESTER_START );
    for(int i = 0; i < cantTestersComunes; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerComun", "testerComun", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterComun de ID" + idTester, __FILE__);
            exit(1);
        }
    }
    //Testers especiales
    const int cantTestersEspeciales = config.ObtenerParametroEntero( CANT_TESTERS_ESPECIALES );
    for(int i = 0; i < cantTestersEspeciales; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerEspecial", "testerEspecial", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterEspecial de ID" + idTester, __FILE__);
            exit(1);
        }
    }
    //Testers config
    for(int i = 0; i < cantTestersEspeciales; i++, idTester++) {
        char param[3];
        sprintf(param, "%d", idTester);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerConfig", "testerConfig", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterConfig de ID" + idTester, __FILE__);
            exit(1);
        }
    }    
    
    // Creo equipo especial
    pid_t eqEspPid = fork();
    if (eqEspPid == 0) {
        execlp("./equipoEspecial", "equipoEspecial", (char*)0);
        Logger::error("Error al ejecutar el programa Equipo Especial", __FILE__);
        exit(1);
    }

    // Creo al tecnico
    pid_t tecPid = fork();
    if(tecPid == 0) {
        execlp("./tecnico", "tecnico", (char*)0);
        Logger::error("Error al ejecutar el programa tecnico", __FILE__);
        exit(1);
    }

    //Creo dispositivos y procesos dispositivos-config
    sleep(1);
    int idDispositivo = config.ObtenerParametroEntero(ID_DISPOSITIVO_START);
    int cantidad_lanzada = 0;
    const int cantDispositivos = config.ObtenerParametroEntero(CANT_DISPOSITIVOS);
    const int minLanzados = config.ObtenerParametroEntero(MINIMOS_LANZADOS);
    const int maxLanzados = config.ObtenerParametroEntero(MAXIMOS_LANZADOS);
    const int cantTiposDispositivo = config.ObtenerParametroEntero(CANT_TIPOS_DISPOSITIVO);
    while (cantidad_lanzada < cantDispositivos){
        int cantidad_a_lanzar = minLanzados + rand() % (maxLanzados - minLanzados + 1);
        if (cantidad_a_lanzar + cantidad_lanzada > cantDispositivos)
            cantidad_a_lanzar = cantDispositivos - cantidad_lanzada;
        for (int i = 0; i < cantidad_a_lanzar; i++){
            char paramId[3];
            sprintf(paramId, "%d", idDispositivo);
            idDispositivo++;
            char paramTipo[3];
            sprintf(paramTipo, "%d", rand() % cantTiposDispositivo );
            pid_t newPid = fork();
            if(newPid == 0) {
                // Inicio el programa correspondiente
                execlp("./dispositivo", "dispositivo", paramId, paramTipo, (char*)0);
                Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
                exit(1);
            }
        }        
        for (int i = 0; i < cantidad_a_lanzar; i++){
            char paramId[3];
            sprintf(paramId, "%d", idDispositivo);
            idDispositivo++;
            char paramTipo[3];
            sprintf(paramTipo, "%d", rand() % cantTiposDispositivo );
            pid_t newPid = fork();
            if(newPid == 0) {
                // Inicio el programa correspondiente
                execlp("./dispositivoConfig", "dispositivoConfig", paramId, paramTipo, (char*)0);
                Logger::error("Error al ejecutar el programa dispositivo-Config de ID" + idDispositivo, __FILE__);
                exit(1);
            }
        }        
        cantidad_lanzada += cantidad_a_lanzar;
        usleep(1000);
    }
    Logger::debug("Programas iniciados correctamente...", __FILE__);
    
}
