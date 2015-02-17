#pragma once

#include "../logger/Logger.h"
#include "common.h"
#include "../ipc/Semaphore.h"
#include <cstring>
#include <sstream>
#include <sys/msg.h>
#include "errno.h"
#include <vector>

class Configuracion;

class AtendedorTesters {
private:
    const int   m_CantTestersEspeciales;
    int         m_ColaDispositivosConfig;
    int         m_ColaRequerimientos;
    int         m_ColaRecibosTests;
    int         m_ColaTestersConfig;
    int         m_ColaTestersEspeciales;
    int         m_ColaTareasEspeciales;
    int         m_IdPrimerTesterEspecial;
    Semaphore   m_SemColaEspeciales;
//Operaciones prohibidas
    AtendedorTesters(const AtendedorTesters& orig);
    AtendedorTesters& operator=(const AtendedorTesters& rv);
public:
    AtendedorTesters( const Configuracion& config );
    ~AtendedorTesters();
    
    TMessageAtendedor recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int tester, int idPrograma);  // Tester -> Disp
    resultado_test_t recibirResultado(int idTester);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado
    void enviarAEspeciales( std::vector<bool> cuales, int idDispositivo, int posicionDispositivo);
    TMessageAssignTE recibirRequerimientoEspecial(int idEsp);
    void enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo);
    
    void enviarReqTestConfig( int idDispositivo, int idTester, int tipoDispositivo );
    TMessageTesterConfig recibirReqTestConfig( int idTester );
    void enviarCambioVariable( int idDispositivo, int idVariable, int nuevoValor, bool ultimo );

    bool destruirComunicacion();

};
