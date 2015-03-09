#ifndef __COMMON_C__
#define __COMMON_C__

//Aqui van las constantes y tipos que deben ser accedidos desde C (el RPC server)

static int ID_DISPOSITIVO = 5;
const int MAX_TESTER_COMUNES = 30;
const int MAX_TESTER_ESPECIALES = 30;
const int MAX_DISPOSITIVOS_EN_SISTEMA = 100;
const int ID_TESTER_COMUN_START = ID_DISPOSITIVO + MAX_DISPOSITIVOS_EN_SISTEMA + 1;
const int ID_TESTER_ESP_START = ID_TESTER_COMUN_START + MAX_TESTER_COMUNES + 1;
const int ID_EQUIPO_ESPECIAL = ID_TESTER_ESP_START + MAX_TESTER_ESPECIALES + 1;
const int CANT_RESULTADOS = MAX_DISPOSITIVOS_EN_SISTEMA;

// Para el server RPC de identificadores
typedef struct TTablaIdTestersDisponibles {
    bool disponibles[MAX_TESTER_COMUNES];
} TTablaIdTestersDisponibles;

typedef struct TTablaIdTestersEspecialesDisponibles {
    bool disponibles[MAX_TESTER_ESPECIALES];
} TTablaIdTestersEspecialesDisponibles;

#endif //__COMMON_C__
