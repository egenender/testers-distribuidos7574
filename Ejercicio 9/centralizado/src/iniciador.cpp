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
#include <vector>

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

template<typename T>
T* crearSharedMemory( const std::string& archivoIpcs,
                                           int id,
                                           const std::string& duenio,
                                           size_t tamanio ){
    key_t shMemKey = ftok( archivoIpcs.c_str(), id );
    if(shMemKey == -1) {
        std::string err("Error al conseguir la key de la shmem de " + duenio +". Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    int shMemId = shmget( shMemKey, tamanio, IPC_CREAT | IPC_EXCL | 0660);
    if( shMemId == -1 ) {
        std::string err("Error al conseguir la memoria compartida de " + duenio + ". Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    void* tmpPtr = shmat( shMemId, NULL, 0 );
    T* pContenido = NULL;
    if ( tmpPtr != (void*) -1 ) {
        pContenido = static_cast<T*> (tmpPtr);
        Logger::debug("Memoria compartida de " + duenio + " creada correctamente", __FILE__);
    } else {
        std::string err = std::string("Error en shmat() de " + duenio + ". Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }
    return pContenido;
}

//Tambien se inicializan aqui
void crearSharedMemories( const std::string& archivoIpcs, const Configuracion& config ){
    //SHM_PLANILLA_GENERAL
    int* pCantDispositivosSiendoTesteados = crearSharedMemory<int>(
        archivoIpcs, config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL), "planilla general", sizeof(int)
    );
    *(pCantDispositivosSiendoTesteados) = 0;
    //SHM_PLANILLA_GENERAL_POSICIONES
    const int maxDispositivos = config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA);
    bool* pIdsPrivadosDispositivos = crearSharedMemory<bool>(
        archivoIpcs, config.ObtenerParametroEntero(SHM_PLANILLA_GENERAL_POSICIONES),
        "planilla general-posiciones", sizeof(bool) * maxDispositivos
    ); 
    for (int i = 0; i < maxDispositivos; i++)
        pIdsPrivadosDispositivos[i] = false;
    //SHM_PLANILLA_CANT_TESTER_ASIGNADOS
    TContadorTesterEspecial* pCantTestersEspecialesAsignados =
        crearSharedMemory<TContadorTesterEspecial>( archivoIpcs, config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TESTER_ASIGNADOS),
                                                    "planilla asignacion testers", sizeof(TContadorTesterEspecial) * maxDispositivos );
    for (int i = 0; i < maxDispositivos; i++) {
        pCantTestersEspecialesAsignados[i].cantTestersEspecialesTerminados = 0;
        pCantTestersEspecialesAsignados[i].cantTestersEspecialesTotal = 0;
    }
    //SHM_PLANILLA_CANT_TAREAS_ASIGNADAS
    TContadorTareaEspecial* pCantTareasEspecialesAsignadas = crearSharedMemory<TContadorTareaEspecial>(
        archivoIpcs, config.ObtenerParametroEntero(SHM_PLANILLA_CANT_TAREAS_ASIGNADAS),
        "planilla asignacion equipo especial", sizeof(TContadorTareaEspecial) * maxDispositivos );
    for (int i = 0; i < maxDispositivos; i++) {
        pCantTareasEspecialesAsignadas[i].cantTareasEspecialesTerminadas = 0;
        pCantTareasEspecialesAsignadas[i].cantTareasEspecialesTotal = 0;
    }
    //SHM_PLANILLA_VARS_START + idDisp
    for( int i=0; i<maxDispositivos; i++ ){
        int idDisp = i+1; //Los ids de dispositivo comienzan en 1
        std::stringstream ss;
        ss << "planilla vars disp " << idDisp;
        TEstadoDispositivo* pEstadoDisp = crearSharedMemory<TEstadoDispositivo>(
            archivoIpcs,
            config.ObtenerParametroEntero(SHM_PLANILLA_VARS_START) + i + 1, 
            ss.str(), sizeof(TEstadoDispositivo) );
        pEstadoDisp->estadoDisp = EPD_LIBRE;
        pEstadoDisp->estadoDispConfig = EPD_LIBRE;
    }
}

void crearSemaforo( const std::string& archivoIpcs, int idSemaforo, int valorInicial ){
    Semaphore sem( archivoIpcs, idSemaforo );
    sem.creaSem();
    sem.iniSem( valorInicial );
}

void crearSemaforos( const std::string& archivoIpcs, const Configuracion& config ){
    //Mutex
    crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_GENERAL), 1 );
    //Usado por PlanillaAsignacionTesterComun, PlanillaAsignacionTesterEspecial
    //y PlanillaAsignacionEquipoEspecial
    crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TESTER_ASIGNADOS), 1 );
    //Sem cola especiales (AtendedorTesters)
    crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_COLA_ESPECIALES), 1 );
    //Sem tareas asignadas ()
    crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_CANT_TAREAS_ASIGNADAS), 1 );
    //Semaforos planilla variables config
    for( int i=0; i<config.ObtenerParametroEntero( MAX_DISPOSITIVOS_EN_SISTEMA ); i++ ){
        int idDisp = i+1;
        crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_MUTEX_PLANILLA_VARS_START) + idDisp, 1 );
        crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_TE_START) + idDisp, 1 );
        crearSemaforo( archivoIpcs, config.ObtenerParametroEntero(SEM_PLANILLA_VARS_CV_START) + idDisp, 1 );
    }
}

void crearCola( const std::string& archivoIpcs, int idCola ){
    key_t key = ftok(archivoIpcs.c_str(), idCola);
    if (msgget(key, 0660 | IPC_CREAT | IPC_EXCL) == -1){
        std::cout << "No se pudo crear la cola de dispositivos: " << strerror(errno)<< std::endl;
    }
}

void crearColas( const std::string& archivoIpcs, const Configuracion& config ){
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_DISPOSITIVOS ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_TESTERS ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_DISPOSITIVOS_CONFIG ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_TESTERS_CONFIG ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_TESTERS_ESPECIALES ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_DESPACHADOR ) );
    crearCola( archivoIpcs, config.ObtenerParametroEntero( MSGQUEUE_REINICIO_TESTEO ) );
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
    crearSharedMemories( archivoIpcs, config );
    crearSemaforos( archivoIpcs, config );
    crearColas( archivoIpcs, config );
}

void lanzarProcesosSistema( const Configuracion& config ) {
   
    // Creo testers
    const int cantTestersComunes = config.ObtenerParametroEntero( CANT_TESTERS_COMUNES );
    const int offsetTestersComunes = config.ObtenerParametroEntero( ID_TESTER_START );
    for(int i = 0; i < cantTestersComunes; i++) {
        char param[3];
        sprintf(param, "%d", offsetTestersComunes + i);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerComun", "testerComun", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterComun de ID" + offsetTestersComunes + i, __FILE__);
            exit(1);
        }
    }
    //Testers especiales
    const int cantTestersEspeciales = config.ObtenerParametroEntero( CANT_TESTERS_ESPECIALES );
    const int offsetTestersEspeciales = config.ObtenerParametroEntero( ID_TESTER_ESPECIAL_START );
    for(int i = 0; i < cantTestersEspeciales; i++) {
        char param[3];
        sprintf(param, "%d", offsetTestersEspeciales + i);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerEspecial", "testerEspecial", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterEspecial de ID" + offsetTestersEspeciales + i, __FILE__);
            exit(1);
        }
    }
    //Testers config
    const int offsetTestersConfig = config.ObtenerParametroEntero( ID_TESTER_CONFIG_START );
    for(int i = 0; i < cantTestersComunes; i++) {
        char param[3];
        sprintf(param, "%d", offsetTestersConfig + i);
        usleep(10);
        pid_t newPid = fork();
        if(newPid == 0) {
            // Inicio el programa correspondiente
            execlp("./testerConfig", "testerConfig", param, (char*)0);
            Logger::error("Error al ejecutar el programa TesterConfig de ID" + offsetTestersConfig + i, __FILE__);
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
    std::vector<int> idsDispositivos;
    std::vector<int> tiposDeDispositivo;
    while (cantidad_lanzada < cantDispositivos){
        int cantidad_a_lanzar = minLanzados + rand() % (maxLanzados - minLanzados + 1);
        if (cantidad_a_lanzar + cantidad_lanzada > cantDispositivos)
            cantidad_a_lanzar = cantDispositivos - cantidad_lanzada;
        std::map<int,int> dispositivoYTipos;
        for (int i = 1; i <= cantidad_a_lanzar; i++){
            char paramId[3];
            sprintf(paramId, "%d", idDispositivo);
            int tipoDispositivo = rand() % cantTiposDispositivo;
            idsDispositivos.push_back(idDispositivo);
            tiposDeDispositivo.push_back(tipoDispositivo);
            idDispositivo++;
            char paramTipo[3];
            sprintf(paramTipo, "%d", tiposDeDispositivo.back() );
            pid_t newPid = fork();
            if(newPid == 0) {
                // Inicio el programa correspondiente
                execlp("./dispositivo", "dispositivo", paramId, paramTipo, (char*)0);
                Logger::error("Error al ejecutar el programa dispositivo de ID" + idDispositivo, __FILE__);
                exit(1);
            }
        }
        for (int i = 1; i <= cantidad_a_lanzar; i++){
            char paramId[3];
            sprintf(paramId, "%d", idsDispositivos[i-1]); //Obtengo el id definido en el ciclo anterior
            char paramTipo[3];
            sprintf(paramTipo, "%d", tiposDeDispositivo[i-1] ); //Obtengo el tipo definido en el ciclo anterior
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
