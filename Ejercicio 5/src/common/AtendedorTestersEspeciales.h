#ifndef ATENDEDORTESTERS_H
#define	ATENDEDORTESTERS_H

#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"

class AtendedorTestersEspeciales {
private:
    int cola_pruebasEspeciales;
        
    typedef struct message {
        long mtype;
        int idDispositivo;
        int value;
    } TMessageAtendedor;
    
public:
    AtendedorTestersEspeciales();
    AtendedorTestersEspeciales(const AtendedorTestersEspeciales& orig);
    virtual ~AtendedorTestersEspeciales();
    
    void enviarPruebaEspecial(int idDispositivo, int tester, int idPrograma);  // Tester Esp -> Disp
    resultado_test_t recibirResultadoPruebaEspecial(int idTester);

    bool destruirComunicacion();

};

#endif	/* ATENDEDORTESTERS_H */

