/*
 * Coordinador.h
 *
 *  Created on: 06/10/2014
 *      Author: aceki
 */

#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <stdlib.h>
#include <list>
#include "ipc/Semaphore.h"

using namespace std;

class Coordinador {

private:
	Semaphore semTesterRequerimiento;
	Semaphore semTesterResultado;
	Semaphore semStatusTesterReq;
	Semaphore semStatusTesterResultado;
	Semaphore semRegistrarDispositivo;

	list<int> dispositivosRegistrados;
	int idDispositivo;
	int statusTesterReq;
	int statusTesterResult;

public:
	Coordinador();
	void registrarDispositivo(int dispositivo);

	int getEstadoTesterResultado();
	int getEstadoTesterRequerimiento();

	int setOcupadoTesterRequerimiento();
	int setDesocupadoTesterRequerimiento();
	int setOcupadoTesterResultado();
	int setDesocupadoTesterResultado();

	virtual ~Coordinador();
};

#endif /* COORDINADOR_H_ */
