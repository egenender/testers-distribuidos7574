/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#ifndef IPLANILLATESTERA_H
#define	IPLANILLATESTERA_H

class iPlanillaTesterA {
private:
        
public:
    iPlanillaTesterA(int idTester);
    iPlanillaTesterA(const iPlanillaTesterA& orig);
    virtual ~iPlanillaTesterA();
    bool agregar(int idDispositivo);
    void terminoRequerimientoPendiente();
    
};

#endif	/* IPLANILLATESTERA_H */

