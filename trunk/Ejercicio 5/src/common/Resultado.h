/* 
 * File:   Resultado.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:28 PM
 */

#ifndef RESULTADO_H
#define	RESULTADO_H

#include "common.h"
#include <cstdlib>
#include <list>

using namespace std;

class Resultado {
public:
    Resultado();
    Resultado(const Resultado& orig);
    virtual ~Resultado();
    
    static int esGrave(int resultado) {return (resultado == RESULTADO_GRAVE);};

    static list<int> getTestersEspecialesIds(int resultado) {
    	list<int> ids;
    	unsigned int cant_testers = (rand() % (MAX_TESTERS_ESP - MIN_TESTERS_ESP) + MIN_TESTERS_ESP);
    	while ( ids.size() != cant_testers ) {
    		ids.push_back( (rand() % CANT_TESTERS_ESPECIALES) + CANT_TESTERS + ID_TESTER_START );
    		ids.unique();
    	}
    	return ids;
    }

    static int getSiguientePrueba(int res) {
    	return rand() % 2; // CONTINUAR TESTEO O FIN TESTEO ESPECIAL
    }

    static resultado_test_t getResultadoFinal(int testerId, int dispositivo, list<resultado_test_t> resultados) {
    	resultado_test_t resFinal;
    	resFinal.tester = testerId;
    	resFinal.result = rand() % 2; //Resultado Grave o No Grave
    	resFinal.dispositivo = dispositivo;
    	return resFinal;
    }
};

#endif	/* RESULTADO_H */

