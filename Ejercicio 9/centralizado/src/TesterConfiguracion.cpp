#include "logger/Logger.h"
#include "common/AtendedorTesters.h"
#include "common/TestConfiguracion.h"
#include "common/Configuracion.h"
#include <cstdlib>

using namespace Constantes::NombresDeParametros;
using namespace std;

int main(int argc, char** argv) {
    // El primer parametro es el id del tester
    int id = atoi(argv[1]);
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    stringstream nombre;
    nombre << __FILE__ << " " << id;
    Logger::notice("Inicia el procesamiento, cargo atendedor" , nombre.str().c_str());
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", nombre.str().c_str());
        return 1;
    }
    
    AtendedorTesters atendedor( config );

    while(true) {
        TMessageTesterConfig reqTest = atendedor.recibirReqTestConfig( id );
        stringstream ss;
        ss << "Requerimiento de test de configuración para disp " << reqTest.idDispositivo << " recibido";
        Logger::notice( ss.str(), nombre.str().c_str() );
        ss.str("");
        TestConfiguracion test( reqTest.tipoDispositivo );
        const int maxValorVariableDisp = config.ObtenerParametroEntero( MAX_VALOR_VARIABLE_DISP );
        int cantVars = test.getCantVariables();
        for( int i=0; i<cantVars; i++ ){
            int nuevoValorVar = rand() % maxValorVariableDisp;
            atendedor.enviarCambioVariable( reqTest.idDispositivo,
                                            test.getVariable(i),
                                            nuevoValorVar,
                                            i == (test.getCantVariables()-1) );
        }
    }
}
