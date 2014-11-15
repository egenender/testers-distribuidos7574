#ifndef PLANILLA_LOCAL_H
#define	PLANILLA_LOCAL_H

const int LIBRE = 0;
const int OCUPADO = 1;
const int ESPERANDO = 2;

typedef struct struct_planilla_local{
    int cantidad;
    int resultados;
    int estadoA;
    int estadoB;
}planilla_local_t;



#endif	/* PLANILLA_LOCAL_H */

