/* 
 * File:   AtendedorDispositivos.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 8:04 PM
 */

#ifndef ATENDEDORDISPOSITIVOS_H
#define	ATENDEDORDISPOSITIVOS_H

class AtendedorDispositivos {
private:
    
public:

    AtendedorDispositivos();
    AtendedorDispositivos(const AtendedorDispositivos& orig);    
    virtual ~AtendedorDispositivos();
    
    // Crea e inicializa IPC
    void crearQueue();
    
    void enviarRequerimiento(int idDispositivo); // Disp -> Tester
    int recibirRequerimiento();
    void enviarPrograma(int idPrograma);  // Tester -> Disp
    int recibirPrograma();
    void enviarResultado(int resultado); // Disp -> Tester: Grave o no grave
    int recibirResultado();
    void enviarOrden(int orden); // Tester -> Disp: Reinicio o apagado
    int recibirOrden();
};

#endif	/* ATENDEDORDISPOSITIVOS_H */

