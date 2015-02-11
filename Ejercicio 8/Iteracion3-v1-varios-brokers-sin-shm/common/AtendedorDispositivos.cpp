/* 
 * File:   AtendedorDispositivos.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 8:04 PM
 */
#include "AtendedorDispositivos.h"

AtendedorDispositivos::AtendedorDispositivos(int idDispositivo) : idDispositivo(idDispositivo) {

    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_DISP);
    this->colaEnvios = msgget(key, 0666);
    if(this->colaEnvios == -1) {
        std::string err = std::string("Error al obtener la cola para enviar mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_DISP);
    this->colaRecepciones = msgget(key, 0666);
    if(this->colaRecepciones == -1) {
        std::string err = std::string("Error al obtener la cola para recibir mensajes. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramIdCola[10];
    char paramId[10];
    char paramSizeMsg[10];

	sprintf(paramIdCola, "%d", MSGQUEUE_RECEPCIONES_DISP);
    sprintf(paramId, "%d", this->idDispositivo);

	this->pidReceptor = fork();
	if (this->pidReceptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR_DISPOSITIVOS,
                paramId, paramIdCola,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
		exit(1);
	}

	sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_DISP);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));

    this->pidEmisor = fork();
	if (this->pidEmisor == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR_DISPOSITIVOS,
				paramId, paramIdCola, paramSizeMsg, 
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}

}

AtendedorDispositivos::AtendedorDispositivos(const AtendedorDispositivos& orig) {
}

AtendedorDispositivos::~AtendedorDispositivos() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", this->pidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}
    
void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = this->idDispositivo;
    msg.mtypeMensaje = MTYPE_REQUERIMIENTO_DISPOSITIVO;
    msg.idDispositivo = idDispositivo;
    
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar requerimiento al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idTester = msg.tester;
    this->idBroker = msg.idBroker;
    return msg.value;
}

void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

	TMessageAtendedor msg;
	msg.mtype = this->idDispositivo;
	msg.mtypeMensaje = MTYPE_RESULTADO_INICIAL;
	msg.tester = this->idTester;
	msg.idDispositivo = idDispositivo;
    msg.idBroker = this->idBroker;
	msg.value = resultado;
            
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirProgramaEspecial(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir programa especial del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idTester = msg.tester;
    this->idBroker = msg.idBroker;
    this->posicionDispositivo = msg.posicionDispositivo;
    return msg.value;
}

// El resultado especial se envia solo al Equipo Especial, por lo que el mtype influye poco
void AtendedorDispositivos::enviarResultadoEspecial(int idDispositivo, int resultado) {

	TMessageAtendedor msg;
		msg.mtype = idDispositivo;
		msg.mtypeMensaje = MTYPE_RESULTADO_ESPECIAL;
		msg.idDispositivo = idDispositivo;
		msg.tester = this->idTester;
        msg.idBroker = this->idBroker;
		msg.value = resultado;
		msg.posicionDispositivo = this->posicionDispositivo;
            
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar resultado especial al sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idDispositivo, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir orden del sistema de testeo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;

}