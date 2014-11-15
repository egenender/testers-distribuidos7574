#include <cstdlib>
#include "common/common.h"
#include "common/Planilla.h"
#include "logger/Logger.h"
#include <sys/msg.h>

using namespace std;

int main(int argc, char** argv) {
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
    
    Planilla planilla(id, TIPO_B);
    requerimiento_planilla_t requerimiento;
    
    Logger::notice("Entrando a loop principal" , nombre.str().c_str());
    while (true){
        if(msgrcv(planilla.queue(), &requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long), id, 0 ) == -1)
			exit(0);
		
        switch(requerimiento.tipoReq){
            case REQUERIMIENTO_ELIMINAR_DISPOSITIVO:
				Logger::notice("Recibo un pedido para eliminar un dispositivo a la planilla" , nombre.str().c_str());
                planilla.eliminar(requerimiento.idDispositivo);
                break;
            case REQUERIMIENTO_INICIAR_PROC_RESULTADOS:
				Logger::notice("Recibo un pedido para que el tester B pueda iniciar el procesamiento de resultados" , nombre.str().c_str());
                planilla.iniciarProcesamientoResultados();
                break;
            case REQUERIMIENTO_PROCESAR_SIGUIENTE:
				Logger::notice("Recibo un pedido para poder procesar el siguiente resultado pendiente" , nombre.str().c_str());
                planilla.procesarSiguiente();
                break;
        }
              
    }
    
    return 0;
}

