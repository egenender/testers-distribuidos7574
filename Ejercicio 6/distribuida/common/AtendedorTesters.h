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
    int cola_requerimiento;
    int cola_recibos_tests;
    int cola_testers_especiales;
    Semaphore sem_cola_especiales;
          
public:
    AtendedorTesters();
    AtendedorTesters(const AtendedorTesters& orig);
    virtual ~AtendedorTesters();
    
    int recibirRequerimiento();
    void enviarPrograma(int idDispositivo, int tester, int idPrograma);  // Tester -> Disp
    int recibirResultado(int idTester);
    void enviarOrden(int idDispositivo, int orden, int cant); // Tester -> Disp: Reinicio o apagado
    void enviarAEspeciales(bool cuales[], int posicion);
	int recibirRequerimientoEspecial(int idEsp); 
    
    void terminar_atencion(int idDispositivo_atendido);

};

#endif	/* ATENDEDORTESTERS_H */

