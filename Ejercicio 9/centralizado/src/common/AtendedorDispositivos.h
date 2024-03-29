/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#pragma once

#include <sys/msg.h>
#include "errno.h"
#include "../common/common.h"
#include <cstdlib>
#include <cstring>
#include "../logger/Logger.h"
#include "common.h"

class Configuracion;

class AtendedorDispositivos {
private:
    int m_ColaDispositivoConfig;
    int m_ColaRequerimientos;
    int m_ColaTests;
    int m_ColaTestsEspeciales;
    int m_PosicionDispositivo;
//Operaciones prohibidas
    AtendedorDispositivos(const AtendedorDispositivos& orig);
    AtendedorDispositivos& operator=(const AtendedorDispositivos& rv);
public:
    AtendedorDispositivos( const Configuracion& config );
    ~AtendedorDispositivos();
        
    void enviarRequerimiento( int idDispositivo, int tipoDispositivo ); // Disp -> Tester
    int recibirPrograma( int idDispositivo, int* pIdTester );
    void enviarResultado( int idDispositivo, int idTester, int resultado ); // Disp -> Tester: Grave o no grave
    int recibirProgramaEspecial( int idDispositivo, int* pIdTesterEspecial );
    void enviarResultadoEspecial( int idDispositivo, int IdTesterEspecial, int resultado );
    int recibirOrden( int idDispositivo );
    
    TMessageDispConfig recibirPedidoCambioVariable( int idDispositivo );
    void notificarCambioDeVariableFinalizado( int idDispositivo, int IdTesterEspecial, bool ultimoCambio );
};
