#pragma once

class Configuracion;

class iPlanillaTesterRespuesta {
private:
    int idTester;
    int cola;
public:
    iPlanillaTesterRespuesta(int id, const Configuracion& config);
    iPlanillaTesterRespuesta(const iPlanillaTesterRespuesta& orig);
    virtual ~iPlanillaTesterRespuesta();
    void eliminarDispositivo(int iDdispositivo);
    void iniciarProcesamientoDeResultados();
    void procesarSiguiente();
    void agregarResultado();

};

