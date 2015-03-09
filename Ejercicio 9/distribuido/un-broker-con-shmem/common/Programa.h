/* 
 * File:   Programa.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 10:24 PM
 */

#pragma once

#include <cstdlib>

class Programa {
public:
    Programa();
    Programa(const Programa& orig);
    virtual ~Programa();
    
    static int getPrograma() {return ((rand() % 100) + 500);};

};
