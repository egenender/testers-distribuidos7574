#include <cstdlib>
#include "ipc/Semaphore.h"

using namespace std;

const int LIBRE = 0;
const int OCUPADO = 1;
const int ESPERANDO = 2;

typedef struct struct_planilla_local{
    int cantidad;
    int resultados;
    int estadoA;
    int estadoB;
}planilla_local_t;


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
        int cantidad = shm_planilla_local->resultados;
        mutex_planilla_local.v();
        if (cantidad == 0){
            sem_tester_B.p();
        }
        
    }
};

int main(int argc, char** argv) {
    int cantidad_dispositivos; //TODO: memoria compartida
    Planilla planilla;
    while (true){
        //TODO leer de la cola (struct c)
        int tipo_req; //TODO: esto se saca del struct
        
        
    }
    
    return 0;
}

