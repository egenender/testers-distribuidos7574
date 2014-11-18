#ifndef ATENDEDORTESTERS_H
#define	ATENDEDORTESTERS_H

#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"

class AtendedorTesters {
private:
    int cola_requerimiento;
    int cola_recibos_tests;
        
    typedef struct message {
        long mtype;
        int idDispositivo;
        int value; // Este parametro posee el valor del requerimiento, del programa y del resultado
    } TMessageAtendedor;
    
public:
    AtendedorTesters();
    AtendedorTesters(const AtendedorTesters& orig);
    virtual ~AtendedorTesters();

    int recibirRequerimiento();
    int recibir2doRequerimiento();
    void enviarPrograma(int idDispositivo, int idTester, int idPrograma);  // Tester -> Disp
    resultado_test_t recibirResultado(int idTester);
    void enviarOrden(int idDispositivo, int orden); // Tester -> Disp: Reinicio o apagado

    bool destruirComunicacion();

};

#endif	/* ATENDEDORTESTERS_H */

