/* 
 * File:   EquipoEspecial.cpp
 * Author: ferno
 *
 * Created on November 17, 2014, 12:51 AM
 */

#include <cstdlib>
#include <sstream>
#include <set>

#include "common/common.h"
#include "common/AtendedorEquipoEspecial.h"
#include "common/PlanillaAsignacionEquipoEspecial.h"
#include "common/PlanillaReinicioEquipoEspecial.h"
#include "common/DespachadorTesters.h"
#include "common/Planilla.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    srand(time(NULL));
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    Logger::debug("Se crea el equipo especial...", __FILE__);
    
    // Para almacenar que testers especiales testeaban a que dispositivo
    std::set<int> controlador[MAX_DISPOSITIVOS_EN_SISTEMA];
    // Para almacenar los resultados de los testeos especiales
    int resultados[MAX_DISPOSITIVOS_EN_SISTEMA];
    for (int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++) {
        resultados[i] = 0;
    }

    AtendedorEquipoEspecial atendedor;
    
    Planilla planillaGeneral(ID_EQUIPO_ESPECIAL);
    
    PlanillaAsignacionEquipoEspecial planillaAsignacion;
    
    PlanillaReinicioEquipoEspecial planillaReinicio;
    
    DespachadorTesters despachador;
    
    Logger::notice("Se inicializan correctamente todos los elementos del Equipo Especial", __FILE__);
    
    std::stringstream ss;
    
    while(true) {
        
        // Equipo especial recibe resultados especiales
        // Cuando detecta que terminan todos los testeos para un dispositivo
        // se fija si deben reiniciarse, y sino, envia la ordena a los tecnicos
        
        TMessageAtendedor resultado = atendedor.recibirResultadoEspecial();
        ss << "Recibi el resultado especial " << resultado.value << " del dispositivo " << resultado.idDispositivo << " de la tarea especial enviada por tester " << resultado.tester;
        Logger::debug(ss.str(), __FILE__);
        ss.str("");
        // Almaceno resultado del testeo especial terminado
        resultados[resultado.posicionDispositivo] += resultado.value;
        // Almaceno el tester que testea al dispositivo
        controlador[resultado.posicionDispositivo].insert(resultado.tester);
        // Registro que termino una tarea especial
        planillaAsignacion.registrarTareaEspecialFinalizada(resultado.posicionDispositivo, resultado.idDispositivo);
        ss << "Se registra tarea especial del dispositivo " << resultado.idDispositivo << ", posicion " << resultado.posicionDispositivo << " enviada por tester especial " << resultado.tester << " con exito";
        Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
        
        if (planillaAsignacion.terminoTesteoEspecial()) {
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
        }
    }
    
    Logger::notice("Termina el Equipo Especial", __FILE__);
    
    return 0;
}

