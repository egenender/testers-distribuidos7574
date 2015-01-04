#ifndef ATENDEDORTESTERS_H
#define	ATENDEDORTESTERS_H

#include <cstring>
#include <sys/msg.h>
#include "errno.h"

#include "../logger/Logger.h"
#include "common.h"
#include "../ipc/Semaphore.h"

class AtendedorTesters {
private:
    int cola_envios;
    int cola_recibos;
    Semaphore sem_cola_especiales;
    int idTester;
    
    void activarTester();  
public:
    AtendedorTesters(int tipo);
    AtendedorTesters(const AtendedorTesters& orig);
    virtual ~AtendedorTesters();
    
    int recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int idPrograma);  // Tester -> Disp
    int recibirResultado();
    void enviarOrden(int idDispositivo, int orden, int cant); // Tester -> Disp: Reinicio o apagado
    void enviarAEspeciales(bool cuales[], int posicion);
	int recibirRequerimientoEspecial(); 
    int obtenerIdTester();
    
    void terminar_atencion(int tipo);

};

#endif	/* ATENDEDORTESTERS_H */

