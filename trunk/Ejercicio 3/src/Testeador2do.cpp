#include <cstdlib>

#include "common/AtendedorDispositivos.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"

using namespace std;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTester2do planilla(id);
        
    while(true) {
        
        
        // Espero un requerimiento
        int idDispositivo = atendedor.recibir2doRequerimiento();
     
        atendedor.enviarPrograma(idDispositivo, id, Programa::getPrograma());
        
        planilla.iniciarProcesamientoResultadosParciales();
        
        planilla.procesarSiguienteResultado();
        
        
    }

    return 0;
}

