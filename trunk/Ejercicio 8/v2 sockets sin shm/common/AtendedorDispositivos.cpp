/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"

AtendedorDispositivos::AtendedorDispositivos() { 

	this->idDispositivo = getIdDispositivo();
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIOS_DISP);
    this->cola_envioS = msgget(key, 0666);
    if(this->cola_envios == -1) {
        std::string err = std::string("Error al obtener la cola para enviar mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_DISP);
    this->cola_recepciones = msgget(key, 0666);
    if(this->cola_recepciones == -1) {
        std::string err = std::string("Error al obtener la cola para recibir mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char param_cola[10];
    char param_id[10];
    char param_pid[10];

    // RECIBIR PROGRAMA, ORDEN, PROGRAMA ESPECIAL
	sprintf(param_cola, "%d", MSGQUEUE_RECEPCIONES_DISP);

	pid_t receptor = fork();
	if (receptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR_DISP,
				param_cola,
				(char*) 0);
		exit(1);
	}


	// ENVIAR REQUERIMIENTO, RESULTADO, RESULTADO ESPECIAL
	sprintf(param_id, "%d", this->idDispositivo);
	sprintf(param_pid, "%d", receptor);
	sprintf(param_cola, "%d", MSGQUEUE_ENVIOS_DISP);

	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR_DISP,
				param_id, param_cola,
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
    msg.mtype = this->idDispositivo;
    msg.mtype_envio = MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.es_requerimiento = 1;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar requerimiento al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idTester = msg.idTester;
    return msg.value;
}

void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

	TMessageAtendedor msg;
	msg.mtype = this->idDispositivo;
	msg.mtype_envio = (long) this->idTester;
	msg.finalizar_conexion = 0;
	msg.tester = this->idTester;
	msg.idDispositivo = idDispositivo;
	msg.value = resultado;
	msg.es_requerimiento = 0;
            
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorDispositivos::recibirProgramaEspecial(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
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

	TMessageAtendedor msg;
		msg.mtype = idDispositivo;
		msg.mtype_envio = MTYPE_RESULTADO_ESPECIAL;
		msg.idDispositivo = idDispositivo;
		msg.idTester = idTester;
		msg.value = resultado;
		msg.posicionDisp = this->posicionDispositivo;
            
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;

}
