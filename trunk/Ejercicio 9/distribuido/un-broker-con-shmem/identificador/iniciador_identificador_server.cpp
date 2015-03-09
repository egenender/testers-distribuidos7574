/*
 * Crea IPC's correspondientes y levanta el servidor de identificadores (RPC)
 */

#include <fstream>
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "common/common.h"
#include "logger/Logger.h"
#include "ipc/Semaphore.h"
#include "common/Configuracion.h"

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
    std::fstream ipcFile(archivoIpcs.c_str(), std::ios_base::out);
    ipcFile.close();
    
    Semaphore semId( archivoIpcs, config.ObtenerParametroEntero(SEM_IDENTIFICADOR) );
    semId.creaSem();
    semId.iniSem(1);
    Semaphore semTablaTestEsp( archivoIpcs, config.ObtenerParametroEntero(SEM_TABLA_TESTERS_ESPECIALES_DISPONIBLES) );
    semTablaTestEsp.creaSem();
    semTablaTestEsp.iniSem(1);
    Semaphore semTablaTestCom( archivoIpcs, config.ObtenerParametroEntero(SEM_TABLA_TESTERS_COMUNES_DISPONIBLES) );
    semTablaTestCom.creaSem();
    semTablaTestCom.iniSem(1);
    
    key_t key = ftok(archivoIpcs.c_str(), SHM_TESTERS_COMUNES_DISPONIBLES);
    int shmTablaTestCom = shmget(key, sizeof(TTablaIdTestersDisponibles), IPC_CREAT | IPC_EXCL | 0660);
    TTablaIdTestersDisponibles* tablaTesterComDisp = (TTablaIdTestersDisponibles*) shmat(shmTablaTestCom, NULL, 0);
    for (int i = 0; i < Constantes::MAX_TESTER_COMUNES; i++)
        tablaTesterComDisp->disponibles[i] = true;
    
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(SHM_TESTERS_ESPECIALES_DISPONIBLES) );
    if (key == -1) {
        std::stringstream ss;
        ss << "Error en la obtencion de la clave para el IPC " << config.ObtenerParametroEntero(SHM_TESTERS_ESPECIALES_DISPONIBLES) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    int shmTablaTestEsp = shmget(key, sizeof(TTablaIdTestersEspecialesDisponibles), IPC_CREAT | IPC_EXCL | 0660);
    if (shmTablaTestEsp == -1) {
        std::stringstream ss;
        ss << "Error en la obtencion del ID para el IPC " << config.ObtenerParametroEntero(SHM_TESTERS_ESPECIALES_DISPONIBLES) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    TTablaIdTestersEspecialesDisponibles* tablaTesterEspDisp = (TTablaIdTestersEspecialesDisponibles*) shmat(shmTablaTestEsp, NULL, 0);
    if (tablaTesterEspDisp == (void*) -1) {
        std::stringstream ss;
        ss << "Error en el attacheo del proceso a la ShMem de testers especiales disponibles con ID " << config.ObtenerParametroEntero(SHM_TESTERS_ESPECIALES_DISPONIBLES) << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    for (int i = 0; i < Constantes::MAX_TESTER_ESPECIALES; i++)
        tablaTesterEspDisp->disponibles[i] = true;

    pid_t idServer = fork();
    if (idServer == 0) {
        execlp("./idServer", "idServer", (char*)0);
        Logger::error("Error luego de ejecutar el idServer", __FILE__);
        exit(1);
    }
    
    return 0;
}
