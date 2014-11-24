#include <cstdlib>
#include "common/common.h"
#include "common/Planilla.h"
#include "logger/Logger.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
	std::stringstream nombre;
	nombre << __FILE__ << " " << id;
	Logger::notice("Inicia el procesamiento, cargando IPCS" , nombre.str().c_str());
		
    Planilla planilla(id, TIPO_A);
    requerimiento_planilla_t requerimiento;
    
    Logger::notice("Entrando a loop principal" , nombre.str().c_str());
    while (true){
        if (msgrcv(planilla.queue(), &requerimiento, sizeof(requerimiento_planilla_t) - sizeof(long), id, 0 ) == -1){
			exit(0);
		}
		stringstream ss;		
        switch(requerimiento.tipoReq){
            case REQUERIMIENTO_AGREGAR:
				ss << "Recibo un pedido para agregar el dispositivo " << requerimiento.idDispositivo << " a la planilla" ;
				Logger::notice( ss.str() , nombre.str().c_str());
				planilla.agregar(requerimiento.idDispositivo);
                break;
            case REQUERIMIENTO_TERMINO_PENDIENTE_REQ:
				Logger::notice("Recibo un pedido para marcar la finalizacion de requerimiento pendiente" , nombre.str().c_str());
                planilla.terminadoRequerimientoPendiente();
                break;
        }
              
    }
    
    return 0;
}

