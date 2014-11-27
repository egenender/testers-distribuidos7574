#include <cstdlib>
#include "common/common.h"
#include "common/Planilla.h"

using namespace std;

int main(int argc, char** argv) {
    int id = atoi(argv[0]);
    Planilla planilla(id);
    requerimiento_planilla_t requerimiento;
    while (true){
        msgrcv(planilla.queue(), &requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long), id, 0 );
        switch(requerimiento.tipoReq){
            case REQUERIMIENTO_INICIAR_PROC_RESULTADOS_PARCIALES:
                planilla.iniciarProcesamientoResultadosParciales();
                break;
            case REQUERIMIENTO_PROCESAR_SIGUIENTE:
                planilla.procesarSiguiente();
                break;
            case REQUERIMIENTO_AGREGAR_RESULTADO_PARCIAL:
                planilla.agregarResultadoParcial();
                break;
        }
              
    }
    
    return 0;
}

