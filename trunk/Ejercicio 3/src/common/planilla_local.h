#pragma once

const int LIBRE = 0;
const int OCUPADO = 1;
const int ESPERANDO = 2;

typedef struct struct_planilla_local{
    int cantidad;
    int resultados;
    int resultadosParciales;
    int estado1;
    int estado2;
    int estadoRes;
}planilla_local_t;

