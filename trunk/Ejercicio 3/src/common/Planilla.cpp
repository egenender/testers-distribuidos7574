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

Planilla::Planilla(int idTester, int tipo) : mutex_planilla_general(SEM_PLANILLA_GENERAL), mutex_planilla_local(SEM_PLANILLA_LOCAL + idTester), sem_tester_A(SEM_TESTER_A + idTester), sem_tester_B(SEM_TESTER_B + idTester) {
    
    this->mutex_planilla_general.getSem();
    this->mutex_planilla_local.getSem();
    this->sem_tester_primero.getSem();
    this->sem_tester_segundo.getSem();
    this->sem_tester_resultado.getSem();

    key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + idTester);
    int shmlocalid = shmget(key, sizeof (planilla_local_t), 0660);
    //verificacion de errores
    this->shm_planilla_local = (planilla_local_t*) shmat(shmlocalid, NULL, 0);

    key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
    int shmgeneralid = shmget(key, sizeof (int), 0660);
    //verificacion de errores
    this->shm_planilla_general = (int*) shmat(shmgeneralid, NULL, 0);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA + tipo);
    this->cola = msgget(key, 0660);

    id = idTester;
    stringstream ss;
    ss << "Planilla component " << id;
    nombre = ss.str();
};

int Planilla::queue() {
    return this->cola;
};

void Planilla::agregar(int idDispositivo) {
    respuesta_lugar_t respuesta;
    respuesta.mtype = idDispositivo;
    this->mutex_planilla_local.p();
    if (*this->shm_planilla_local == MAX_DISPOSITIVOS_POR_TESTER) {
        mutex_planilla_local.v();
        respuesta.respuesta = false;
        Logger::notice("No hay lugar en la planilla", nombre);
        if (msgsnd(cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0) == -1)
            exit(0);
        return;
    }
    mutex_planilla_general.p();
    *shm_planilla_general = *shm_planilla_general + 1;
    stringstream ss;
    ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
    Logger::notice(ss.str(), __FILE__);
    this->mutex_planilla_general.v();
    
    *shm_planilla_local->cantidad++;
    ss << "Ahora hay " << *shm_planilla_local->cantidad << " dispositivos en la planilla "<< id;
    Logger::notice(ss.str(), __FILE__);

    this->shm_planilla_local->cantidad++;
    if (this->shm_planilla_local->estadoRes == LIBRE && this->shm_planilla_local->estado2 == LIBRE) {
        Logger::notice("El tester de resultados y el 2do estan libres, asi que el primer tester puede trabajar", nombre);
        this->shm_planilla_local->estado1 = OCUPADO;
        this->mutex_planilla_local.v();
    } else {
        Logger::notice("El tester 1 tiene que esperar porque los otros no estan ambos libres", nombre);
        this->shm_planilla_local->estado1 = ESPERANDO;
        
        if (this->shm_planilla_local->estadoRes == ESPERANDO && this->shm_planilla_local->estado2 != OCUPADO ) {
            this->shm_planilla_local->estadoRes = OCUPADO;
            this->sem_tester_resultado.v();
        }
        
        if (this->shm_planilla_local->estadoRes == LIBRE && this->shm_planilla_local->estado2 == ESPERANDO ) {
            this->shm_planilla_local->estado2 = OCUPADO;
            this->sem_tester_segundo.v();
        }
        
        this->mutex_planilla_local.v();
        this->sem_tester_primero.p();
    }

    respuesta.respuesta = true;
    if (msgsnd(this->cola, &respuesta, sizeof (respuesta_lugar_t) - sizeof (long), 0) == -1)
        exit(0);
};

void Planilla::terminadoRequerimientoPendiente() {
    this->mutex_planilla_local.p();
    Logger::notice("El tester 1 ahora esta libre", nombre);
    this->shm_planilla_local->estado1 = LIBRE;
    if (this->shm_planilla_local->estadoRes == ESPERANDO) {
        Logger::notice("El tester de resultados estaba esperando, asi que lo libero", nombre);
        this->shm_planilla_local->estadoB = OCUPADO;
        this->sem_tester_resultado.v();
    } else {
        if (this->shm_planilla_local->estado2 == ESPERANDO) {
        Logger::notice("El tester 2do estaba esperando, asi que lo libero", nombre);
        this->shm_planilla_local->estado2 = OCUPADO;
        this->sem_tester_segundo.v();
        } else {
        Logger::notice("El tester 2 y el de resultados ninguno estaba esperando, asi que no hago nada", nombre);
        }
    }
    this->mutex_planilla_local.v();
};

void Planilla::procesarSiguiente() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultados > 0) {
        stringstream ss;
        ss << "En procesar siguiente Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
        Logger::notice(ss.str(), nombre);
        this->mutex_planilla_local.v();
        return;
    }
    Logger::notice("En procesar siguiente, no hay resultados pendientes me fijo el estado de Tester 2", nombre);
    this->shm_planilla_local->estadoRes = LIBRE;
    if (this->shm_planilla_local->estado2 == ESPERANDO) {
        Logger::notice("Tester 2 estaba esperando, asi que ahora va a poder trabajar", nombre);
        this->shm_planilla_local->estado2 = OCUPADO;
        this->sem_tester_segundo.v();
    } else {
        if (this->shm_planilla_local->estado1 == ESPERANDO) {
        Logger::notice("Tester 1 estaba esperando, asi que ahora va a poder trabajar", nombre);
        this->shm_planilla_local->estado1 = OCUPADO;
        this->sem_tester_primero.v();
        } else {
        Logger::notice("Ningun Tester estaba esperando, asi que no hay nada que hacer", nombre);
        }
    }
    this->mutex_planilla_local.v();
};

void Planilla::iniciarProcesamientoResultados() {
    this->mutex_planilla_local.p();
    if (this->shm_planilla_local->resultados == 0) {
        Logger::notice("No hay resultados pendientes, asi que el tester B queda libre", nombre);
        this->shm_planilla_local->estadoB = LIBRE;
        this->mutex_planilla_local.v();
        this->sem_tester_B.p();
    } else {
        stringstream ss;
        ss << "Quedan " << this->shm_planilla_local->resultados << " resultados pendientes";
        Logger::notice(ss.str(), nombre);
        this->shm_planilla_local->estadoB = OCUPADO;
        this->mutex_planilla_local.v();
    }
    this->mutex_planilla_local.p();
    this->shm_planilla_local->resultados--;
    this->mutex_planilla_local.v();
};

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

void Planilla::eliminar(int idDispositivo) {
    this->mutex_planilla_local.p();
    this->shm_planilla_local->cantidad--;
    this->mutex_planilla_local.v();

    this->mutex_planilla_general.p();
    *shm_planilla_general = *shm_planilla_general - 1;
    stringstream ss;
    ss << "Ahora hay " << *shm_planilla_general << " dispositivos en el sistema";
    Logger::notice(ss.str(), nombre);
    this->mutex_planilla_general.v();
}


void Planilla::agregarResultado(){
    
}

void Planilla::agregarResultadoParcial(){
    
    
}