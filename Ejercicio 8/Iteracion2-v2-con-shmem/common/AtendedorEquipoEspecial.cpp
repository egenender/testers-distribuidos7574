/* 
 * File:   AtendedorEquipoEspecial.cpp
 * Author: ferno
 * 
 * Created on November 17, 2014, 12:59 AM
 */

#include "AtendedorEquipoEspecial.h"

AtendedorEquipoEspecial::AtendedorEquipoEspecial() {
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL);
    this->colaRecepciones = msgget(key, 0666);
    if(this->colaRecepciones == -1) {
        std::string err = std::string("Error al obtener la cola de recepciones del equipo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_EQUIPO_ESPECIAL);
    this->colaEnvios = msgget(key, 0666);
    if(this->colaEnvios == -1) {
        std::string err = std::string("Error al obtener la cola de envios del equipo especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    char paramIdCola[10];
    char paramId[10];

	sprintf(paramIdCola, "%d", MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL);
    sprintf(paramId, "%d", ID_EQUIPO_ESPECIAL);

	this->pidReceptor = fork();
	if (this->pidReceptor == 0) {
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER, PUERTO_SERVER_EMISOR,
                paramId, paramIdCola,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_receptor. Error!", __FILE__);
		exit(1);
	}

	sprintf(paramIdCola, "%d", MSGQUEUE_ENVIO_EQUIPO_ESPECIAL);

	if (fork() == 0) {
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER, PUERTO_SERVER_RECEPTOR,
				paramId, paramIdCola,
				(char*) 0);
        Logger::error("Log luego de execlp tcpclient_emisor. Error!", __FILE__);
		exit(1);
	}
}

AtendedorEquipoEspecial::~AtendedorEquipoEspecial() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", this->pidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}

TMessageAtendedor AtendedorEquipoEspecial::recibirResultadoEspecial() {
    TMessageAtendedor resultado;
    int ret = msgrcv(this->colaRecepciones, &resultado, sizeof(TMessageAtendedor) - sizeof(long), 0, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado especial de algun dispositivo. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return resultado;
}

void AtendedorEquipoEspecial::enviarFinTestEspecialADispositivo(int idDispositivo) {
    TMessageAtendedor msg;
    msg.mtype = ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = MTYPE_FIN_TEST_ESPECIAL;
    msg.idDispositivo = idDispositivo;
    msg.value = FIN_TEST_ESPECIAL;
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar fin de testeo especial al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorEquipoEspecial::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = ID_EQUIPO_ESPECIAL;
    msg.mtypeMensaje = MTYPE_ORDEN;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::ostringstream ss;
        ss << "Error al enviar orden de apagado o reinicio al dispositivo " << idDispositivo << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}