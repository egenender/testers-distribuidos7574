#include "logger/Logger.h"
#include "common/AtendedorTesters.h"
#include "TestConfiguracion.h"
#include <cstdlib>

int main(int argc, char** argv) {
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    std::stringstream nombre;
    nombre << __FILE__ << " " << id;
    Logger::notice("Inicia el procesamiento, cargo atendedor" , nombre.str().c_str());
    
    AtendedorTesters atendedor;
    
    while(true) {
        TMessageConfigTest reqTest = atendedor.recibirReqTestConfiguracion();
        TestConfiguracion test( reqTest.TipoDispositivo );
        for( int i=0; i<test.CantVariables(); i++ ){
            atendedor.enviarTestConfiguracion( reqTest.idDispositivo, test.getVariable(i) );
            TMessageResultadoConfigTest resultado = atendedor.recibirResultadoTestConfig( reqTest.idDispositivo );
            //TODO <NIM> Procesar resultado del test
        }
    }
}
