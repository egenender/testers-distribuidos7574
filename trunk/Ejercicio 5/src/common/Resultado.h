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
    	int cant_testers = rand() % (MAX_TESTERS_ESP - MIN_TESTERS_ESP) + MIN_TESTERS_ESP;
    	while ( ids.size() == cant_testers ) {
    		ids.push_back( rand() % CANT_TESTERS_ESPECIALES );
    		ids.unique();
    	}
    	return ids;
    }
};

#endif	/* RESULTADO_H */

