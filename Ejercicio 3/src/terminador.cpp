#include <cstdlib>
#include <common/common.h>
#include "unistd.h"

#include "logger/Logger.h"
#include "common/AtendedorTesters.h"
#include "common/Planilla.h"
#include "common/DespachadorTecnicos.h"


void createIPCObjects();
void createSystemProcesses();

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::error("Logger inicializado. Destruyendo IPCs...", __FILE__);
   
    // Creo objeto despachador para remover la cola de mensajes
    DespachadorTecnicos despachador;
    if (!despachador.destruirComunicacion()) {
	Logger::error("No se pudo destruir la cola de mensajes del despachador...", __FILE__);
    }

    // Creo objeto atendedor para remover la cola de mensajes
    AtendedorTesters atendedor;
    if (!atendedor.destruirComunicacion()) {
        Logger::error("No se pudo destruir la cola de mensajes del atendedor...", __FILE__);
    }

    // Creo objeto planilla para destruir el semaforo y la memoria compartida que utiliza
    Planilla planilla;
    if (!planilla.destruirMemoria()) {
	Logger::error("No se pudo destruir la memoria compartida de la planilla...", __FILE__);
    }
    if (!planilla.destruirSemaforo()) {
	std::string err = std::string("No se pudo destruir el semaforo de la planilla. Error: ") + std::string(strerror(errno));
	Logger::error(err.c_str(), __FILE__);
    }

    unlink(ipcFileName.c_str());
 
    Logger::notice("IPCs eliminados...", __FILE__);
    
    Logger::destroy();
    
    return 0;
}
