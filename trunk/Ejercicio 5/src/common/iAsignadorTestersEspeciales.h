#ifndef IASIGNADORTESTERSESPECIALES_H
#define	IASIGNADORTESTERSESPECIALES_H

class iAsignadorTestersEspeciales {

private:
	//cola para mandar los pedidos de testers especiales
	//cola para recibir todos los diagnosticos
	//semaforo para bloquear el comportamiento del tester
	//shmemory para anotar y descontar los testers especiales

public:
	iAsignadorTestersEspeciales();
	iAsignadorTestersEspeciales(&iasignador);
	
	int asignar(list<int>* idsTestersEspeciales);
	list<int> getTodosLosDiagnosticos();
	
	~iAsignadorTestersEspeciales();
};

#endif /* ASIGNADORTESTERSESPECIALES_H */
