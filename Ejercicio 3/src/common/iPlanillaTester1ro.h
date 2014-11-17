/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#ifndef IPLANILLATESTERA_H
#define	IPLANILLATESTERA_H

class iPlanillaTester1ro {
private:
    int idTester;
    int cola;
public:
    iPlanillaTester1ro(int id);
    iPlanillaTester1ro(const iPlanillaTester1ro& orig);
    virtual ~iPlanillaTester1ro();
    bool agregar(int idDispositivo);
    void terminoRequerimientoPendiente();
    
};

#endif	/* IPLANILLATESTERA_H */

