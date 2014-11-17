/* 
 * File:   AtendedorTesters.h
 * Author: knoppix
 *
 * Created on October 7, 2014, 8:54 PM
 */

#pragma once

class Configuracion;

#include <sys/ipc.h>

class AtendedorTesters {
private:
    int m_MsgQueueId;
    key_t m_Key;

    typedef struct message {
        long mtype;
        int idDispositivo;
        int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
    } TMessageAtendedor;

//Prohibo copia y asignacion
    AtendedorTesters(const AtendedorTesters& orig);
    AtendedorTesters& operator=(const AtendedorTesters& rv);

    int recibirReq( int tipoMsg );
public:
    AtendedorTesters( const Configuracion& config );
    ~AtendedorTesters();

    int recibirRequerimiento();
    int recibirRequerimiento( int idTester );
    void enviarPrograma(int idDispositivo, int idPrograma);  // Tester -> Disp
    int recibirResultado(int idDispositivo);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado

    bool destruirComunicacion();

};

