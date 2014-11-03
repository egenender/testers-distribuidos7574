/* 
 * File:   DespachadorTesters.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 10:30 PM
 */

#include "AsignadorTestersEspeciales.h"
#include "../logger/Logger.h"
#include <cstdlib>

AsignadorTestersEspeciales::AsignadorTestersEspeciales() {

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_B_ESP);
    this->msgQueueId = msgget(key, 0666 | IPC_CREAT); 
    if(this->msgQueueId == -1) {
		std::string error = std::string("Error creando la cola de mensajes del asignador. Errno = ") + std::string(strerror(errno));
        Logger::error(error, __FILE__);
    }
    
}

AsignadorTestersEspeciales::AsignadorTestersEspeciales(const AsignadorTestersEspeciales& orig) {
}

AsignadorTestersEspeciales::~AsignadorTestersEspeciales() {
}

list<resultado_test_t> AsignadorTestersEspeciales::asignar(int idTester, int idDispositivo, list<int> testersEspecialesIds) {

	list<resultado_test_t> resultList;
	int cantidadResultadosAEsperar = testersEspecialesIds.size();

	// Enviamos los pedidos
	for (list<int>::iterator it=testersEspecialesIds.begin(); it != testersEspecialesIds.end(); ++it) {
		TMessageAsignador msg;
		msg.mtype = *it;
		msg.idDispositivo = idDispositivo;
		msg.value = idTester; //se lo envio para que me lo devuelvan
    
		int ret = msgsnd(this->msgQueueId, &msg, sizeof(TMessageAsignador) - sizeof(long), 0);
		if (ret == -1) {
			std::string error = std::string("Error al enviar pedido a los testers especiales. Error: ") + std::string(strerror(errno));
			Logger::error(error.c_str(), __FILE__);
			throw error;
		}
	}

	// Esperamos los resultados
	while ( cantidadResultadosAEsperar != 0 ) {
		TMessageAsignador msg;
	    int ret = msgrcv(this->msgQueueId, &msg, sizeof(TMessageAsignador) - sizeof(long), idTester, 0);
		if (ret == -1) {
	        std::string error("Error al recibir el resultado del asignador. Error: " + errno);
	        Logger::error(error.c_str(), __FILE__);
	        exit(0);
	    }
		cantidadResultadosAEsperar--;

		resultado_test_t resultParcial;
		resultParcial.tester = idTester;
		resultParcial.dispositivo = idDispositivo;
		resultParcial.result = msg.value;
		resultList.push_back(resultParcial);
	}

	return resultList;
}
