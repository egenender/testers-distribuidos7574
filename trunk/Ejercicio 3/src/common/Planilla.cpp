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

Planilla::Planilla(int idTester, const Configuracion& config) :
        mutex_planilla_general( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ),
                                config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_GENERAL) ),
        mutex_planilla_local( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ),
                              config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_PLANILLA_LOCAL) + idTester ),
        sem_tester_primero( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ),
                            config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_A) + idTester ),
        sem_tester_segundo( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ),
                            config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_B) + idTester ),
        sem_tester_resultado( config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ),
                              config.ObtenerParametroEntero(Constantes::NombresDeParametros::SEM_TESTER_RESULTADO) + idTester ),
        cola( -1 ),
        m_IdShmLocal( -1 ),
        m_IdShmGeneral( -1 ),
        m_MaxDispositivosLocales( config.ObtenerParametroEntero(Constantes::NombresDeParametros::MAX_DISPOSITIVOS_POR_TESTER) ) {
    //Semaforos
    this->mutex_planilla_general.getSem();
    this->mutex_planilla_local.getSem();
    this->sem_tester_primero.getSem();
    this->sem_tester_resultado.getSem();
    this->sem_tester_segundo.getSem();

    std::stringstream ss;
    //Shared memory local
    const std::string archivoIpcs = config.ObtenerParametroString( Constantes::NombresDeParametros::ARCHIVO_IPCS ).c_str();
    key_t key = ftok( archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::SHM_PLANILLA_LOCAL) + idTester);
    if(key == -1) {
        std::string err("Error al conseguir la key de la shmem local de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_IdShmLocal = shmget(key, sizeof (planilla_local_t), IPC_CREAT);
    if(m_IdShmLocal == -1) {
        std::string err("Error al conseguir la memoria compartida local de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shm_planilla_local = static_cast<planilla_local_t*>( shmat(m_IdShmLocal, NULL, 0) );
    if ( this->shm_planilla_local != (void*) -1 ) {
        ss << "Memoria compartida local de la planilla creada correctamente con id " << m_IdShmLocal;
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
    } else {
        std::string err = std::string("Error en shmat() de memoria local de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    //Shared memory general
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::SHM_PLANILLA_GENERAL));
    if(key == -1) {
        std::string err("Error al conseguir la key de la shmem general de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    m_IdShmGeneral = shmget(key, sizeof (int), IPC_CREAT);
    if(m_IdShmGeneral == -1) {
        std::string err("Error al conseguir la memoria compartida general de la planilla. Error: " + std::string(strerror(errno)));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    this->shm_planilla_general = static_cast<int*>( shmat(m_IdShmGeneral, NULL, 0) );
    if ( this->shm_planilla_general != (void*) -1 ) {
        ss << "Memoria compartida local de la planilla creada correctamente con id " << m_IdShmGeneral;
        Logger::debug(ss.str().c_str(), __FILE__);
        ss.str("");
    } else {
        std::string err = std::string("Error en shmat() de memoria general de la planilla. Error: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        throw err;
    }

    //Cola de mensajes
    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_PLANILLA));
    this->cola = msgget(key, IPC_CREAT);
}

int Planilla::queue() {
    return this->cola;
}

void Planilla::agregar(int aQuien) {
    respuesta_lugar_t respuesta;
    respuesta.mtype = aQuien;
    this->mutex_planilla_local.p();
    if ( this->shm_planilla_local->cantidad == m_MaxDispositivosLocales ) {
        mutex_planilla_local.v();
        respuesta.respuesta = false;
        msgsnd(cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0);
        return;
    }

    this->shm_planilla_local->cantidad++;

    if (this->shm_planilla_local->estadoB == LIBRE) {
        this->shm_planilla_local->estadoA = OCUPADO;
        this->mutex_planilla_local.v();
    } else {
        this->shm_planilla_local->estadoA = ESPERANDO;
        this->mutex_planilla_local.v();
        this->sem_tester_primero.p();
    }

    respuesta.respuesta = true;
    msgsnd(this->cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0);
}

void Planilla::terminadoRequerimientoPendiente() {
    this->mutex_planilla_local.p();
    this->shm_planilla_local->estadoA = LIBRE;
    if (this->shm_planilla_local->estadoB == ESPERANDO) {
        this->shm_planilla_local->estadoB = OCUPADO;
        this->sem_tester_resultado.v();
    }
    this->mutex_planilla_local.v();
}

void Planilla::procesarSiguiente() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultados > 0) {
        this->mutex_planilla_local.v();
        return;
    }
    this->shm_planilla_local->estadoB = LIBRE;
    if (this->shm_planilla_local->estadoA == ESPERANDO) {
        this->shm_planilla_local->estadoA = OCUPADO;
        this->sem_tester_primero.v();
    }
    this->mutex_planilla_local.v();
}

void Planilla::iniciarProcesamientoResultados() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultados == 0) {
        this->shm_planilla_local->estadoB = LIBRE;
        this->mutex_planilla_local.v();
        this->sem_tester_resultado.p();
    } else {
        this->shm_planilla_local->estadoB = OCUPADO;
        this->mutex_planilla_local.v();
    }
    this->mutex_planilla_local.p();
    this->shm_planilla_local->resultados--;
    this->mutex_planilla_local.v();
}

//HAY QUE ARREGLAR BIEN ESTE METODO ESTUDIANDO LOS SEM

void Planilla::iniciarProcesamientoResultadosParciales() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultadosParciales == 0) {
        this->shm_planilla_local->estadoB = LIBRE;
        this->mutex_planilla_local.v();
        this->sem_tester_resultado.p();
    } else {
        if (this->shm_planilla_local->resultados != 0) {
            this->shm_planilla_local->estadoB = ESPERANDO;
            this->mutex_planilla_local.v();
        } else {
            this->shm_planilla_local->estadoB = OCUPADO;
            this->mutex_planilla_local.v();
        }
    }
    this->mutex_planilla_local.p();
    this->shm_planilla_local->resultados--;
    this->mutex_planilla_local.v();
}

void Planilla::eliminar(int disp) {
    this->mutex_planilla_local.p();
    this->shm_planilla_local->cantidad--;
    this->mutex_planilla_local.v();

    this->mutex_planilla_general.p();
    (*this->shm_planilla_general) = (*this->shm_planilla_general) - 1;
    this->mutex_planilla_general.v();
}

bool Planilla::destruirCola(){
    return msgctl(this->cola, IPC_RMID, (struct msqid_ds*)0) != -1;
}

bool Planilla::destruirMemoriaGeneral() {
    return (shmctl(m_IdShmGeneral, IPC_RMID, NULL) != -1);
}

bool Planilla::destruirMemoriaLocal() {
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


