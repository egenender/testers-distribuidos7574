#pragma once

class iPlanillaTesterRespuesta {
private:
    int idTester;
    int cola;
public:
    iPlanillaTesterRespuesta(int id);
    iPlanillaTesterRespuesta(const iPlanillaTesterRespuesta& orig);
    virtual ~iPlanillaTesterRespuesta();
    void eliminarDispositivo(int iDdispositivo);
    void iniciarProcesamientoDeResultados();
    void procesarSiguiente();

};

