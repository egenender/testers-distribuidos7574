/* 
 * File:   Programa.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:24 PM
 */

#ifndef PROGRAMA_H
#define	PROGRAMA_H

#include <cstdlib>
#include <time.h>       /* time */

class Programa {
public:
    Programa();
    Programa(const Programa& orig);
    virtual ~Programa();
    
    static int getPrograma() {srand (time(NULL)); return rand() % 100;};

};

#endif	/* PROGRAMA_H */

