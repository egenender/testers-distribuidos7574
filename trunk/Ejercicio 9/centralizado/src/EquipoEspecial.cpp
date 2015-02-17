/* 
 * File:   EquipoEspecial.cpp
 * Author: knoppix
 *
 * Created on November 17, 2014, 12:51 AM
 */

#include "common/common.h"
#include "common/AtendedorEquipoEspecial.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include "common/PlanillaReinicioEquipoEspecial.h"
#include "common/DespachadorTesters.h"
#include "common/Planilla.h"
#include "common/PlanillaVariablesDisp.h"
#include "common/Configuracion.h"
#include <cstdlib>
#include <sstream>
#include <set>
#include <vector>

using namespace Constantes::NombresDeParametros;

int main(int argc, char** argv) {
    
    srand(time(NULL));
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::debug("Se crea el equipo especial...", __FILE__);
    
    Configuracion config;
    if( !config.LeerDeArchivo() ){
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        return 1;
    }
    const int maxDispositivosEnSistema = config.ObtenerParametroEntero(MAX_DISPOSITIVOS_EN_SISTEMA);
    
    // Para almacenar que testers especiales testeaban a que dispositivo
    std::vector< std::set<int> > controlador(maxDispositivosEnSistema);
    // Para almacenar los resultados de los testeos especiales
    std::vector<int> resultados(maxDispositivosEnSistema);
    for (int i = 0; i < maxDispositivosEnSistema; i++) {
        resultados[i] = 0;
    }

    AtendedorEquipoEspecial atendedor( config );
    
    Planilla planillaGeneral( config );
    
    PlanillaAsignacionEquipoEspecial planillaAsignacion( config );
    
    PlanillaReinicioEquipoEspecial planillaReinicio( config );
    
    DespachadorTesters despachador( config );
    
    Logger::notice("Se inicializan correctamente todos los elementos del Equipo Especial", __FILE__);
    
    std::stringstream ss;
    
    bool testConfigCompleto = false;
    
    while(true) {
        
        // Equipo especial recibe resultados especiales
        // Cuando detecta que terminan todos los testeos para un dispositivo
        // se fija si deben reiniciarse, y sino, envia la ordena a los tecnicos
        
        TResultadoEspecial resultado = atendedor.recibirResultadoEspecial();
        
        switch( resultado.mtype ){
            case MTYPE_CAMBIO_VAR:
                ss << "Recibi el cambio de variable del dispositivo " << resultado.idDispositivo << " efectuado por tester " << resultado.idTester;
                Logger::debug(ss.str(), __FILE__);
                ss.str("");
                testConfigCompleto = ( resultado.resultado == FIN_TEST_CONFIG );
                break;
            case MTYPE_RESULTADO_ESPECIAL:
                ss << "Recibi el resultado especial " << resultado.resultado << " del dispositivo " << resultado.idDispositivo << " de la tarea especial enviada por tester " << resultado.idTester;
                Logger::debug(ss.str(), __FILE__);
                ss.str("");
                // Almaceno resultado del testeo especial terminado
                resultados[resultado.posicionDispositivo] += resultado.resultado;
                // Almaceno el tester que testea al dispositivo
                controlador[resultado.posicionDispositivo].insert(resultado.idTester);
                // Registro que termino una tarea especial
                planillaAsignacion.registrarTareaEspecialFinalizada(resultado.posicionDispositivo);
                Logger::debug("Se registra la tarea especial terminada con exito", __FILE__);
                break;
            default:
                Logger::error( "Equipo especial recibio mensaje de mtype invalido", __FILE__ );
                exit( 0 );
                break;
        }

        if ( planillaAsignacion.terminoTesteoEspecial(resultado.posicionDispositivo) && testConfigCompleto ) {
            ss << "Termino el testeo especial del dispositivo " << resultado.idDispositivo << ". Se verificara si hay que rehacerlo";
            Logger::notice(ss.str(), __FILE__);
            ss.str("");
            // Si se ha terminado el testeo especial para este dispositivo
            // me fijo si hay que reiniciarlo, y si no, envio ordenes
            Logger::debug("Se reiniciaron los contadores del testeo especial", __FILE__);
            if (resultados[resultado.posicionDispositivo] % 5 == 0) {
                planillaAsignacion.reiniciarContadoresTesteoEspecial(resultado.posicionDispositivo);
                ss << "Hay que reiniciar el testeo para el dispositivo " << resultado.idDispositivo;
                Logger::notice(ss.str(), __FILE__);
                ss.str("");
                planillaReinicio.avisarReinicio(controlador[resultado.posicionDispositivo], true);
                Logger::debug("Se aviso envio mensaje de reinicio del testeo a los dispositivos correspondientes", __FILE__);
            } else {
                ss << "Se termina el testeo para el dispositivo " << resultado.idDispositivo;
                Logger::debug( ss.str(), __FILE__);
                ss.str("");
                // Aviso al dispositivo que ya no recibira mas tests especiales
                atendedor.enviarFinTestEspecialADispositivo(resultado.idDispositivo);
                // Aviso a los testers especiales que no tienen que rehacer el test
                planillaReinicio.avisarReinicio(controlador[resultado.posicionDispositivo], false);
                // Limpio todos los contadores asignados al dispositivo
                planillaAsignacion.limpiarContadoresFinTesteo(resultado.posicionDispositivo);
                // Envio orden correspondiente dependiendo del resultado
                if (resultados[resultado.posicionDispositivo] % 5 == RESULTADO_GRAVE) {
                    ss << "Se envia orden de apagado al dispositivo y a los tecnicos para el dispositivo " << resultado.idDispositivo;
                    Logger::debug(ss.str(), __FILE__);
                    ss.str("");
                    despachador.enviarOrden(resultado.idDispositivo);
                    atendedor.enviarOrden(resultado.idDispositivo, ORDEN_APAGADO);
                } else {
                    ss << "Se envia orden de reinicio al dispositivo " << resultado.idDispositivo;
                    Logger::debug(ss.str(), __FILE__);
                    ss.str("");
                    atendedor.enviarOrden(resultado.idDispositivo, ORDEN_REINICIO);
                }
                planillaGeneral.eliminarDispositivo(resultado.posicionDispositivo);
                ss << "Elimino al dispositivo " << resultado.idDispositivo << " de la planilla general y sigo procesando...";
                Logger::debug(ss.str(), __FILE__);
                ss.str("");
            }
            // Reinicio los resultados y los testers especiales asignados
            resultados[resultado.posicionDispositivo] = 0;
            controlador[resultado.posicionDispositivo].clear();
            testConfigCompleto = false;
        }
    }
    
    Logger::notice("Termina el Equipo Especial", __FILE__);
    
    return 0;
}

