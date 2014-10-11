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
    
public:
    Planilla(int idTester);
    
    void agregar(/*Struct*/){
        mutex_planilla_general.p();
        if (*shm_planilla_general == MAX_DISPOSITIVOS_EN_SISTEMA){
            mutex_planilla_general.v();
            //TODO:enviar a la cola el 'no hay lugar'
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
        
        //TODO:enviar a la cola el 'si hay lugar'
        return;
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
    int cantidad_dispositivos; //TODO: memoria compartida
    Planilla planilla;
    while (true){
        //TODO leer de la cola (struct c)
        int tipo_req; //TODO: esto se saca del struct
        switch(tipo_req){
            case REQUERIMIENTO_AGREGAR:
                planilla.agregar();
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

