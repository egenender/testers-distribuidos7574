/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#pragma once

class Configuracion;

class iPlanillaTester1ro {
private:
    int idTester;
    int cola;
    
//Prohibo copia y asignacion
    iPlanillaTester1ro(const iPlanillaTester1ro& orig);
    iPlanillaTester1ro& operator=(const iPlanillaTester1ro& rv);

public:
    iPlanillaTester1ro(int id, const Configuracion& config);
    ~iPlanillaTester1ro();
    bool agregar(int idDispositivo);
    void terminoRequerimientoPendiente(int idDispositivo);
    
};
