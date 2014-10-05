#include "Programa.h"
#include <cstdlib>

program_t Programa::getPrograma() const{
	program_t programa;
	programa.tiempo = rand() % (MAXIMO_TIEMPO +1) + MINIMO_TIEMPO;
	programa.prob = rand() % 100;
}
