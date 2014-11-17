/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#pragma once

#include <sys/ipc.h>

class Configuracion;

class AtendedorDispositivos {
private:
    int m_MsgQueueId;
    key_t m_Key;

    typedef struct message {
        long mtype;
        int idDispositivo;
        int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
    } TMessageAtendedor;

//Prohibo copia y asignacion
    AtendedorDispositivos(const AtendedorDispositivos& orig);
    AtendedorDispositivos& operator=(const AtendedorDispositivos& rv);

    void enviarReq( int tipoMsg, int idDisp );
public:

    AtendedorDispositivos( const Configuracion& config );
    ~AtendedorDispositivos();

    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    void enviarRequerimiento(int idTester, int idDispositivo); //Para enviar a un tester especifico
    int recibirPrograma(int idDispositivo);
    void enviarResultado(int idDispositivo, int resultado); // Disp -> Tester: Grave o no grave
    int recibirOrden(int idDispositivo);
    bool destruirComunicacion();
};

