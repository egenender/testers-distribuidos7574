#include "Planilla.h"
#include "Configuracion.h"
#include <cstdlib>
#include "ipc/Semaphore.h"
#include "logger/Logger.h"
#include "planilla_local.h"
#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <cerrno>
#include <cstring>


using namespace std;

Planilla::Planilla(int idTester, const Configuracion& config) :
mutex_planilla_general(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS),
config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_GENERAL)),
mutex_planilla_local(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS),
config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_LOCAL) + idTester),
sem_tester_primero(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS),
config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_A) + idTester),
sem_tester_segundo(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS),
config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_B) + idTester),
sem_tester_resultado(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS),
config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_RESULTADO) + idTester),
cola(-1),
m_IdShmLocal(-1),
m_IdShmGeneral(-1),
id(idTester){
    
    Logger::initialize(Constantes::ARCHIVO_LOG.c_str(), Logger::LOG_DEBUG);
    //Semaforos
    this->mutex_planilla_general.getSem();
    this->mutex_planilla_local.getSem();
    this->sem_tester_primero.getSem();
    this->sem_tester_resultado.getSem();
    this->sem_tester_segundo.getSem();

    std::stringstream ss;
    //Shared memory local
    const std::string archivoIpcs = config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str();
    key_t key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::SHM_PLANILLA_LOCAL) + idTester);
    if (key == -1) {
        std::string err("Error al conseguir la key de la shmem local de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_IdShmLocal = shmget(key, sizeof (planilla_local_t), IPC_CREAT);
    if (m_IdShmLocal == -1) {
        std::string err("Error al conseguir la memoria compartida local de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shm_planilla_local = static_cast<planilla_local_t*> (shmat(m_IdShmLocal, NULL, 0));
    if (this->shm_planilla_local != (void*) - 1) {
        ss << "Memoria compartida local de la planilla creada correctamente con id " << m_IdShmLocal << " para el tester " << idTester;
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
    } else {
        std::string err = std::string("Error en shmat() de memoria local de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    //Shared memory general
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::SHM_PLANILLA_GENERAL));
    if (key == -1) {
        std::string err("Error al conseguir la key de la shmem general de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_IdShmGeneral = shmget(key, sizeof (int), IPC_CREAT);
    if (m_IdShmGeneral == -1) {
        std::string err("Error al conseguir la memoria compartida general de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shm_planilla_general = static_cast<int*> (shmat(m_IdShmGeneral, NULL, 0));
    if (this->shm_planilla_general != (void*) - 1) {
        ss << "Memoria compartida general de la planilla creada correctamente con id " << m_IdShmGeneral << " para el tester " << idTester;
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
    } else {
        std::string err = std::string("Error en shmat() de memoria general de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    //Cola de mensajes
    key = ftok(config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS).c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_PLANILLA));
    this->cola = msgget(key, 0666 | IPC_CREAT);
    
 //<DBG>
    ss << "MSGQUEUE_PLANILLA creada con id " << cola;
    Logger::notice( ss.str().c_str(), __FILE__ );
    ss.str("");
    
    this->mutex_planilla_local.p();
    this->shm_planilla_local->cantidad=0;
    this->shm_planilla_local->resultados=0;
    this->shm_planilla_local->resultadosParciales=0;
    this->shm_planilla_local->estado1=LIBRE;
    this->shm_planilla_local->estado2=LIBRE;
    this->shm_planilla_local->estadoRes=LIBRE;
    this->mutex_planilla_local.v();
    
    
}

int Planilla::queue() {
    return this->cola;
}

void Planilla::agregar(int idDispositivo) {
    respuesta_lugar_t respuesta;
    respuesta.mtype = idDispositivo + Constantes::OFFSET_LUGAR;
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->cantidad > Constantes::MAX_DISPOSITIVOS_POR_TESTER) {
        mutex_planilla_local.v();
        respuesta.respuesta = false;
        Logger::notice("No hay lugar en la planilla", __FILE__);
        if (msgsnd(cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0) == -1){
        
            Logger::error("SE CIERRA POR ERROR EN LA COLA", __FILE__);    
            exit(0);
        }
        return;
    }
    mutex_planilla_general.p();
    *shm_planilla_general = *shm_planilla_general + 1;
    stringstream ss;
    ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");
    this->mutex_planilla_general.v();

    shm_planilla_local->cantidad++;
    ss << "Ahora hay " << shm_planilla_local->cantidad << " dispositivos en la planilla " << id;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");

    this->shm_planilla_local->cantidad++;
    if (this->shm_planilla_local->estadoRes == LIBRE && this->shm_planilla_local->estado2 == LIBRE) {
        Logger::notice("El tester de resultados y el 2do estan libres, asi que el primer tester puede trabajar", __FILE__);
        this->shm_planilla_local->estado1 = OCUPADO;
        this->mutex_planilla_local.v();
    } else {
        Logger::notice("El tester 1 tiene que esperar porque los otros no estan ambos libres", __FILE__);
        this->shm_planilla_local->estado1 = ESPERANDO;

        if (this->shm_planilla_local->estadoRes == ESPERANDO && this->shm_planilla_local->estado2 != OCUPADO) {
            this->shm_planilla_local->estadoRes = OCUPADO;
            this->sem_tester_resultado.v();
        }

        if (this->shm_planilla_local->estadoRes == LIBRE && this->shm_planilla_local->estado2 == ESPERANDO) {
            this->shm_planilla_local->estado2 = OCUPADO;
            this->sem_tester_segundo.v();
        }

        this->mutex_planilla_local.v();
        this->sem_tester_primero.p();
    }

    respuesta.respuesta = true;
    if (msgsnd(this->cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0) == -1){
        Logger::error("SE CIERRA POR ERROR EN LA COLA", __FILE__);   
        exit(0);
    }
}

void Planilla::terminadoRequerimientoPendiente() {
    this->mutex_planilla_local.p();
    Logger::notice("El tester 1 ahora esta libre", __FILE__);
    this->shm_planilla_local->estado1 = LIBRE;
    if (this->shm_planilla_local->estadoRes == ESPERANDO) {
        Logger::notice("El tester de resultados estaba esperando, asi que lo libero", __FILE__);
        this->shm_planilla_local->estadoRes = OCUPADO;
        this->sem_tester_resultado.v();
    } else {
        if (this->shm_planilla_local->estado2 == ESPERANDO) {
            Logger::notice("El tester 2do estaba esperando, asi que lo libero", __FILE__);
            this->shm_planilla_local->estado2 = OCUPADO;
            this->sem_tester_segundo.v();
        } else {
            Logger::notice("El tester 2 y el de resultados ninguno estaba esperando, asi que no hago nada", __FILE__);
        }
    }
    this->mutex_planilla_local.v();
}

void Planilla::procesarSiguiente() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultados > 0) {
        stringstream ss;
        ss << "En procesar siguiente Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");
        this->mutex_planilla_local.v();
        return;
    }
    Logger::notice("En procesar siguiente, no hay resultados pendientes me fijo el estado de Tester 2", __FILE__);
    this->shm_planilla_local->estadoRes = LIBRE;
    if (this->shm_planilla_local->estado2 == ESPERANDO) {
        Logger::notice("Tester 2 estaba esperando, asi que ahora va a poder trabajar", __FILE__);
        this->shm_planilla_local->estado2 = OCUPADO;
        this->sem_tester_segundo.v();
    } else {
        if (this->shm_planilla_local->estado1 == ESPERANDO) {
            Logger::notice("Tester 1 estaba esperando, asi que ahora va a poder trabajar", __FILE__);
            this->shm_planilla_local->estado1 = OCUPADO;
            this->sem_tester_primero.v();
        } else {
            Logger::notice("Ningun Tester estaba esperando, asi que no hay nada que hacer", __FILE__);
        }
    }
    this->mutex_planilla_local.v();
}

void Planilla::iniciarProcesamientoResultados() {
    this->mutex_planilla_local.p();

    if (this->shm_planilla_local->resultados == 0) {
        Logger::notice("No hay resultados pendientes, asi que el tester de resultados queda libre", __FILE__);
        this->shm_planilla_local->estadoRes = LIBRE;
        this->sem_tester_resultado.p();

        if (this->shm_planilla_local->resultadosParciales == 0) {
            Logger::notice("No hay resultados parciales pendientes, asi que el tester 2 queda libre", __FILE__);
            this->shm_planilla_local->estado2 = LIBRE;
            this->sem_tester_segundo.p();
        } else {

            stringstream ss;
            ss << "Quedan " << this->shm_planilla_local->resultadosParciales << " resultados parciales pendientes";
            Logger::notice(ss.str().c_str(), __FILE__);
            ss.str("");
            this->shm_planilla_local->estado2 = OCUPADO;
            this->shm_planilla_local->resultadosParciales--;

        }

        this->mutex_planilla_local.v();

    } else {
        stringstream ss;
        ss << "Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");
        this->shm_planilla_local->estadoRes = OCUPADO;
        this->shm_planilla_local->resultados--;
        this->mutex_planilla_local.v();
    }

}

void Planilla::iniciarProcesamientoResultadosParciales() {
    this->iniciarProcesamientoResultados();
}

void Planilla::eliminar(int idDispositivo) {
    this->mutex_planilla_local.p();
    this->shm_planilla_local->cantidad--;
    this->mutex_planilla_local.v();

    this->mutex_planilla_general.p();
    *shm_planilla_general = *shm_planilla_general - 1;
    stringstream ss;
    ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");
    this->mutex_planilla_general.v();
}

void Planilla::agregarResultado() {

    this->mutex_planilla_local.p();
    this->shm_planilla_local->resultados++;
    this->mutex_planilla_local.v();
}

void Planilla::agregarResultadoParcial() {

    this->mutex_planilla_local.p();
    this->shm_planilla_local->resultadosParciales++;
    this->mutex_planilla_local.v();


}


bool Planilla::destruirCola(){
    return msgctl(this->cola, IPC_RMID, (struct msqid_ds*)0) != -1;
}

bool Planilla::destruirMemoriaGeneral() {
    if( shmdt( this->shm_planilla_general ) == -1 )
        return false;
    return (shmctl(m_IdShmGeneral, IPC_RMID, NULL) != -1);
}

bool Planilla::destruirMemoriaLocal() {
    if( shmdt( this->shm_planilla_local ) == -1 )
        return false;
    return (shmctl(m_IdShmLocal, IPC_RMID, NULL) != -1);
}

bool Planilla::destruirSemaforoGeneral(){
    return mutex_planilla_general.eliSem();
}

bool Planilla::destruirSemaforosLocales( std::string& msjError ) {

    msjError = "";
    if( !mutex_planilla_local.eliSem() )
        msjError += "No se pudo eliminar el semaforo de la planilla local";
    if( !sem_tester_primero.eliSem() )
        msjError += "No se pudo eliminar el semaforo del primer tester";
    if( !sem_tester_segundo.eliSem() )
        msjError += "No se pudo eliminar el semaforo del segundo tester";
    if( !sem_tester_resultado.eliSem() )
        msjError += "No se pudo eliminar el semaforo de los resultados";
    return msjError == "";
}

