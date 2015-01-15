#ifndef DISTR_SEM_H
#define DISTR_SEM_H
#include "../common/common.h"

/* Pide la memoria compartida desde donde este, para quien */
void semaforoDistribuido_P(tabla_testers_disponibles_t* tabla, int id_sub_broker);

/* Devuelve la memoria compartida */
void semaforoDistribuido_V(tabla_testers_disponibles_t* tabla, int id_sub_broker);

#endif
