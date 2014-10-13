#include <cstdlib>
#include "ipc/Semaphore.h"
#include "common/planilla_local.h"

using namespace std;

class Planilla{
private:
    Semaphore mutex_planilla_general;
    int* shm_planilla_general;
    
    Semaphore mutex_planilla_local;
    planilla_local_t* shm_planilla_local;
        
    Semaphore sem_tester_A;
    Semaphore sem_tester_B;
    
    int cola;
    
public:
    Planilla(int idTester): mutex_planilla_general(SEM_PLANILLA_GENERAL), mutex_planilla_local(SEM_PLANILLA_LOCAL + idTester), sem_tester_A(SEM_TESTER_A), sem_tester_B(SEM_TESTER_B) {
        mutex_planilla_general.getSem();
        mutex_planilla_local.getSem();
        sem_tester_A.getSem();
        sem_tester_B.getSem();
        
        key_t key = ftok(ipcFileName.c_str(), SHM_PLANILLA_LOCAL + idTester);
        int shmlocalid = shmget(key, sizeof(planilla_local_t), 0660);
        //verificacion de errores
        shm_planilla_local = shmat(shmlocalid, NULL, 0);
        
        key = ftok(ipcFileName.c_str(), SHM_PLANILLA_GENERAL);
        int shmlocalid = shmget(key, sizeof(int), 0660);
        //verificacion de errores
        shm_planilla_general = shmat(shmlocalid, NULL, 0);
        
        key = ftok(ipcFileName.c_str(), MSGQUEUE_PLANILLA);
        cola = msgget(key, IPC_CREAT);
    }
    
    int queue(){
        return cola;
    }
    
    void agregar(int aQuien){
        respuesta_lugar_t respuesta;
        respuesta.mtype = aQuien;
        mutex_planilla_general.p();
        if (*shm_planilla_general == MAX_DISPOSITIVOS_EN_SISTEMA){
            mutex_planilla_general.v();
            respuesta.respuesta = false;           
            msgsnd(cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0);
            return;
        }            
        *shm_planilla_general++;
        mutex_planilla_general.v();
        
        mutex_planilla_local.p();
        
        shm_planilla_local->cantidad++;
        
        if (shm_planilla_local->estadoB == LIBRE){
            shm_planilla_local->estadoA = OCUPADO;
            mutex_planilla_local.v();
        }else{
            shm_planilla_local->estadoA = ESPERANDO;
            mutex_planilla_local.v();
            sem_tester_A.p();
        }
        
        respuesta.respuesta = true;
        msgsnd(cola, &respuesta, sizeof(respuesta_lugar_t) - sizeof(long), 0);
    }
    
    void terminadoRequerimientoPendiente(){        
        mutex_planilla_local.p();
        shm_planilla_local->estadoA = LIBRE;
        if (shm_planilla_local->estadoB == ESPERANDO){
            shm_planilla_local->estadoB = OCUPADO;
            sem_tester_B.v();
        }
        mutex_planilla_local.v();
    }
    
    void procesarSiguiente(){
        mutex_planilla_local.p();
        if (*shm_planilla_local->resultados > 0){
            mutex_planilla_local.v();
            return;
        }
        shm_planilla_local->estadoB = LIBRE;
        if (shm_planilla_local->estadoA == ESPERANDO){
            shm_planilla_local->estadoA = OCUPADO;
            sem_tester_A.v();
        }
        mutex_planilla_local.v();
    }
    
    void iniciarProcesamientoResultados(){
        mutex_planilla_local.p();
        if (shm_planilla_local->resultados == 0){
            shm_planilla_local->estadoB = LIBRE;
            mutex_planilla_local.v();
            sem_tester_B.p();
        }else{
            shm_planilla_local->estadoB = OCUPADO;
            mutex_planilla_local.v();
        }
        mutex_planilla_local.p();
        shm_planilla_local->resultados--;
        mutex_planilla_local.v();
    }
    
    void eliminar(){
        mutex_planilla_local.p();
        shm_planilla_local->cantidad--;
        mutex_planilla_local.v();
        
        mutex_planilla_general.p();
        *shm_planilla_general--;
        mutex_planilla_general.v();
    }
};

int main(int argc, char** argv) {
    Planilla planilla(atoi(argv[0]));
    requerimiento_planilla_t requerimiento;
    while (true){
        msgrcv(planilla.queue(), &requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long), 0 /*cual leer?*/, 0 );
        switch(requerimiento.tipoReq){
            case REQUERIMIENTO_AGREGAR:
                planilla.agregar(requerimiento.idDispositivo);
                break;
            case REQUERIMIENTO_ELIMINAR_DISPOSITIVO:
                planilla.eliminar();
                break;
            case REQUERIMIENTO_INICIAR_PROC_RESULTADOS:
                planilla.iniciarProcesamientoResultados();
                break;
            case REQUERIMIENTO_PROCESAR_SIGUIENTE:
                planilla.procesarSiguiente();
                break;
            case REQUERIMIENTO_TERMINO_PENDIENTE_REQ:
                planilla.terminadoRequerimientoPendiente();
                break;
        }
              
    }
    
    return 0;
}

