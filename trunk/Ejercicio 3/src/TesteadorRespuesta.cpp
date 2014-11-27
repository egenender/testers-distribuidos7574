#include <cstdlib>

#include "common/AtendedorDispositivos.h"
#include "common/Planilla.h"
#include "common/Programa.h"
#include "common/Resultado.h"
#include "common/DespachadorTecnicos.h"
#include "logger/Logger.h"
#include "common/DespachadorTesters.h"

using namespace std;

int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    
    // Obtengo comunicacion con los dispositivos
    AtendedorTesters atendedor;
    // Obtengo planilla general de sync con otros tester
    iPlanillaTesterRespuesta planilla(id);
    // Obtengo comunicacion con los tecnicos
    DespachadorTesters despachador;
    
    while(true) {
    
        resultado_test_t resultado = atendedor.recibirResultado(id);
        
        //meto el resultado en la planilla 
        
        planilla.iniciarProcesamientoDeResultados();
        
        int orden;
        if(Resultado::esGrave(resultado)) {
            despachador.enviarOrden(resultado.dispositivo);
            orden = ORDEN_APAGADO;
        } else {
            orden = ORDEN_REINICIO;
        }
        atendedor.enviarOrden(resultado.dispositivo,orden);
        
        planilla.eliminarDispositivo(resultado.dispositivo);
        planilla.procesarSiguiente();
    }

    return 0;
}
