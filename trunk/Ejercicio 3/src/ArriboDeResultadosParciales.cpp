
#include <common/common.h>
#include "ipc/Semaphore.h"
#include "common/Configuracion.h"
#include "common/planilla_local.h"
#include "logger/Logger.h"
#include <sys/msg.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "logger/Logger.h"

using namespace Constantes;
using namespace std;

int main(int argc, char** argv) {
    int idTester = atoi(argv[1]);

    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    Logger::notice("SE CREO ARRIBO DE RESULTADOS PARCIALES", __FILE__);

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    const std::string archivoIpcs = config.ObtenerParametroString(NombresDeParametros::ARCHIVO_IPCS);

    Semaphore mutex_planilla_local(archivoIpcs.c_str(),
            config.ObtenerParametroEntero(NombresDeParametros::SEM_PLANILLA_LOCAL) + idTester);
    mutex_planilla_local.creaSem();

    key_t key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(NombresDeParametros::SHM_PLANILLA_LOCAL) + idTester);
    int shmlocalid = shmget(key, sizeof (planilla_local_t), 0660);
    std::stringstream ss;
    ss << "Shm local creada con id " << shmlocalid << " por arribo de resultados tester " << idTester;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");
    planilla_local_t* shm_planilla_local = (planilla_local_t*) shmat(shmlocalid, NULL, 0);

    Semaphore sem_tester_segundo(archivoIpcs.c_str(),
            config.ObtenerParametroEntero(NombresDeParametros::SEM_TESTER_B));
    Semaphore sem_tester_resultado(archivoIpcs.c_str(),
            config.ObtenerParametroEntero(NombresDeParametros::SEM_TESTER_RESULTADO));

    //Las colas parecerian estar al reves, pero lo que hacen es justamente ponerlo
    //en la cola correcta
    key = ftok(archivoIpcs.c_str(),
            config.ObtenerParametroEntero(NombresDeParametros::MSGQUEUE_ESCRITURA_RESULTADOS));
    int cola_lectura = msgget(key, 0666 | IPC_CREAT);

    //<DBG>
    ss << "MSGQUEUE_ESCRITURA_RESULTADOS creada con id " << cola_lectura;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");

    key = ftok(archivoIpcs.c_str(),
            config.ObtenerParametroEntero(NombresDeParametros::MSGQUEUE_LECTURA_RESULTADOS));
    int cola_escritura = msgget(key, 0666 | IPC_CREAT);

    ss << "MSGQUEUE_LECTURA_RESULTADOS creada con id " << cola_escritura;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");


    long idTesterResultado = idTester + config.ObtenerParametroEntero("Tester2doIdOffset");

    while (true) {
        resultado_test_t resultado;
        if (-1 == msgrcv(cola_lectura, &resultado, sizeof (TMessageAtendedor )- sizeof (long), idTesterResultado, 0)) {

            std::string error = std::string("Error al hacer msgrcv. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(-1);
        }
        mutex_planilla_local.p();
        shm_planilla_local->resultadosParciales++;

        if (-1 == msgsnd(cola_escritura, &resultado, sizeof (TMessageAtendedor) - sizeof (long), 0)) {


            std::string error = std::string("Error al hacer msgsnd. Error: ") + std::string(strerror(errno));
            Logger::error(error.c_str(), __FILE__);
            exit(-1);

        }

        if (shm_planilla_local->estado2 == LIBRE) {
            if (shm_planilla_local->estadoRes == OCUPADO || shm_planilla_local->estadoRes == ESPERANDO
                    || shm_planilla_local->estado1 == OCUPADO) {
                shm_planilla_local->estado2 = ESPERANDO;
                if (shm_planilla_local->estadoRes == ESPERANDO && shm_planilla_local->estado1 != OCUPADO) {
                    shm_planilla_local->estadoRes = OCUPADO;
                    sem_tester_resultado.v();
                }
            } else {
                shm_planilla_local->estado2 = OCUPADO;
                sem_tester_segundo.v();
            }
        }
        mutex_planilla_local.v();
    }
    return 0;
}
