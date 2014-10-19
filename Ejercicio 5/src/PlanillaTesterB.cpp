#include <cstdlib>
#include "common/common.h"

using namespace std;

int main(int argc, char** argv) {
    int id = atoi(argv[0]);
    Planilla planilla(id);
    requerimiento_planilla_t requerimiento;
    while (true){
        msgrcv(planilla.queue(), &requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long), id, 0 );
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

