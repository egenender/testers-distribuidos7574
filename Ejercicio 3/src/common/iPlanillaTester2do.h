/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#pragma once

class iPlanillaTester2do {
private:
    int idTester;
    int cola;
public:
    iPlanillaTester2do(int id);
    iPlanillaTester2do(const iPlanillaTester2do& orig);
    virtual ~iPlanillaTester2do();
    void iniciarProcesamientoDeResultados();
    void procesarSiguiente();

    
};

