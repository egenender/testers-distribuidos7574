/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#ifndef IPLANILLATESTERA_H
#define	IPLANILLATESTERA_H

class iPlanillaTester2do {
private:
    int idTester;
    int cola;
public:
    iPlanillaTester2do(int id);
    iPlanillaTester2do(const iPlanillaTester2do& orig);
    virtual ~iPlanillaTester2do();
    void iniciarProcesamientoDeResultadosParciales();
    void procesarSiguiente();

    
};

#endif	/* IPLANILLATESTERA_H */

