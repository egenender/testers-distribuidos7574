/* 
 * File:   iPlanillaTesterA.h
 * Author: knoppix
 *
 * Created on October 11, 2014, 2:06 PM
 */

#pragma once

class Configuracion;

class iPlanillaTester2do {
private:
    int idTester;
    int cola;
//Prohibo copia y asignacion
    iPlanillaTester2do(const iPlanillaTester2do& orig);
    iPlanillaTester2do& operator=(const iPlanillaTester2do& rv);
public:
    iPlanillaTester2do(int id, const Configuracion& config);
    
    virtual ~iPlanillaTester2do();
    void iniciarProcesamientoDeResultadosParciales();
    void procesarSiguiente();
    void agregarResultadoParcial();
};
