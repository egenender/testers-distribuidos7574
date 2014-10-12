/* 
 * File:   iPlanillaTesterB.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:07 PM
 */

#ifndef IPLANILLATESTERB_H
#define	IPLANILLATESTERB_H

class iPlanillaTesterB {
private:
    
public:
    iPlanillaTesterB();
    iPlanillaTesterB(const iPlanillaTesterB& orig);
    virtual ~iPlanillaTesterB();
    void eliminarDispositivo(int iDdispositivo);
    void iniciarProcesamientoDeResultados();
    void procesarSiguiente();

};

#endif	/* IPLANILLATESTERB_H */

