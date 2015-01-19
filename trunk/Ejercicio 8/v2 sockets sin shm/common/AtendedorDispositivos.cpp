/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"

AtendedorDispositivos::AtendedorDispositivos() { 
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS);
    this->cola_requerimiento = msgget(key, 0666);
    if(this->cola_requerimiento == -1) {
        std::string err = std::string("Error al obtener la cola para enviar requerimientos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS);
    this->cola_tests = msgget(key, 0666);
    if(this->cola_tests == -1) {
        std::string err = std::string("Error al obtener la cola para enviar testeos a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES);
    this->cola_tests_especiales = msgget(key, 0666);
    if(this->cola_tests_especiales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar tareas especiales a los dispositivos. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    /**
     * Refactor que queda horrible esto aca
     */
    // Set up socket: toma del socket el programa enviado por el tester y lo pone en la cola de
    //respuestas de los Testers
    char param_cola[10];
	sprintf(param_cola, "%d", MSGQUEUE_TESTERS);
	pid_t receptor = fork();
	if (receptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER, PUERTO_SERVER_EMISOR_DISPOSITIVOS, param_cola,
				(char*) 0);
		exit(1);
	}
	// Set up socket: toma el requerimiento puesto por el disp en la cola de req y lo manda por el
	//socket hacia los testers
	char param_id[10];
	sprintf(param_id, "%d", this->idDispositivo);
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS);

	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR_DISPOSITIVOS, param_id, param_cola,
				param_pid, (char*) 0);
		exit(1);
	}

	//set up socket: toma del socket las pruebas especiales de los testers especiales y las coloca
	// en la cola de testers especiales
	sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES);
	pid_t receptorEspeciales = fork();
	if (receptorEspeciales == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER, PUERTO_SERVER_EMISOR_DISPOSITIVOS, param_cola,
				(char*) 0);
		exit(1);
	}

	//Set up socket: toma los resultados especiales del dispositivo y los manda al socket
	sprintf(param_id, "%d", this->idDispositivo);
	sprintf(param_pid, "%d", receptorEspeciales);
	sprintf(param_cola, "%d", MSGQUEUE_DISPOSITIVOS_TESTERS_ESPECIALES);

	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor", UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR_DISPOSITIVOS, param_id, param_cola,
				param_pid, (char*) 0);
		exit(1);
	}

}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    msg.idTester = 0;
    msg.value = 0;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar requerimiento al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idTester = msg.idTester;
    return msg.value;
}

void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    resultado_test_t resultado_test;
    
    resultado_test.mtype = this->idTester;
    resultado_test.idDispositivo = idDispositivo;
    resultado_test.result = resultado;
            
    int ret = msgsnd(this->cola_tests, &resultado_test, sizeof(resultado_test_t) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirProgramaEspecial(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_tests_especiales, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa especial del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idTester = msg.idTester;
    this->posicionDispositivo = msg.posicionDispositivo;
    return msg.value;
}

// El resultado especial se envia solo al Equipo Especial, por lo que el mtype influye poco
void AtendedorDispositivos::enviarResultadoEspecial(int idDispositivo, int resultado) {

    TResultadoEspecial resultadoTestEspecial;
    
    resultadoTestEspecial.mtype = MTYPE_RESULTADO_ESPECIAL;
    resultadoTestEspecial.idDispositivo = idDispositivo;
    resultadoTestEspecial.idTester = this->idTester;
    resultadoTestEspecial.posicionDispositivo = this->posicionDispositivo;
    resultadoTestEspecial.resultado = resultado;
            
    int ret = msgsnd(this->cola_tests_especiales, &resultadoTestEspecial, sizeof(TResultadoEspecial) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;

}
