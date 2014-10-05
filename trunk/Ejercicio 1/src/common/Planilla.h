/* 
 * File:   Planilla.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:08 PM
 * 
 * Registra cantidad de dispositivos siendo atendidos actualmente en el sistema
 */

#ifndef PLANILLA_H
#define	PLANILLA_H

class Planilla {
private:
public:
    
    Planilla();
    Planilla(const Planilla& orig);
    virtual ~Planilla();
    
    // Crea e inicializa IPC shmem
    void crearShMem();
    
    bool hayLugar();    // Si hay menos de 100 -> incrementa en 1 al contador
    void eliminarDispositivo(); // Simplemente decrementa en 1 el contador en la shmem

};

#endif	/* PLANILLA_H */

