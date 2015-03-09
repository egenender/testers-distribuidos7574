/*
 * Destruyo IPCs correspondientes y remuevo el archivo
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "common/common.h"
#include "common/Configuracion.h"
#include "logger/Logger.h"

using namespace Constantes::NombresDeParametros;
using std::string;

int main(int argc, char* argv[]) {

    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);

    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    const string archivoIpcs = config.ObtenerParametroString( ARCHIVO_IPCS );

    key_t key = ftok( archivoIpcs.c_str(),
                      config.ObtenerParametroEntero(SEM_IDENTIFICADOR) );
	int semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES) );
	semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES) );
	semId = semget(key, 1, IPC_CREAT | 0666);
    semctl(semId, 1, IPC_RMID);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SHM_TESTERS_COMUNES_DISPONIBLES) );
    int shmTablaTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    shmctl(shmTablaTestCom, IPC_RMID, NULL);
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(SHM_TESTERS_ESPECIALES_DISPONIBLES) );
    int shmTablaTestEsp = shmget(key, sizeof(TTablaIdTestersDisponibles), 0660);
    shmctl(shmTablaTestEsp, IPC_RMID, NULL);
    
//    remove(archivoIpcs.c_str());
}
