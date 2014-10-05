/* 
 * File:   DespachadorTecnicos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:30 PM
 */

#ifndef DESPACHADORTECNICOS_H
#define	DESPACHADORTECNICOS_H

class DespachadorTecnicos {
public:
    DespachadorTecnicos();
    DespachadorTecnicos(const DespachadorTecnicos& orig);
    virtual ~DespachadorTecnicos();
    
    // Crea e inicializa msgQueue
    void crearQueue();
    
    void enviarOrden(int idDispositivo);
    int recibirOrden();

};

#endif	/* DESPACHADORTECNICOS_H */

