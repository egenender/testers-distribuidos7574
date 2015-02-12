/* 
 * File:   AtendedorTestersEspeciales.cpp
 * Author: ferno
 * 
 * Created on February 5, 2015, 1:02 AM
 */

#include "AtendedorTestersEspeciales.h"

AtendedorTestersEspeciales::AtendedorTestersEspeciales(int idTester) : idTester(idTester) {
    
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTER_ESPECIAL);
    this->colaEnvios = msgget(key, 0666);
    if(this->colaEnvios == -1) {
		std::string err = std::string("Error al obtener la cola de envios del tester especial. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL);
    this->colaRecepciones = msgget(key, 0666);
    if(this->colaRecepciones == -1) {
        std::string err = std::string("Error al obtener la cola de recepcion del tester especial. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramSizeMsg[10];
    char paramId[10];
    sprintf(paramId, "%d", this->idTester);
    char paramCola[10];
    sprintf(paramCola, "%d", MSGQUEUE_RECEPCIONES_TESTER_ESPECIAL);
    this->pidReceptor = fork();
    if (this->pidReceptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR,
				paramId, paramCola,(char*)0);
        exit(1);
	}
	
    sprintf(paramCola, "%d", MSGQUEUE_ENVIO_TESTER_ESPECIAL);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));

	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR,
				paramId, paramCola, paramSizeMsg, 
                (char*)0);
        exit(1);
	}
    
    registrarTester();
}

AtendedorTestersEspeciales::~AtendedorTestersEspeciales() {
    char pidToKill[10];
    sprintf(pidToKill, "%d", this->pidReceptor);
    if (fork() == 0) {
        execlp("/bin/kill", "kill", pidToKill, (char*) 0);
    }
}

TMessageAtendedor AtendedorTestersEspeciales::recibirRequerimientoEspecial(int idEsp) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), idEsp, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    this->idBroker = msg.idBroker;
    std::stringstream ss; ss << "Le asigne a mi id Broker " << this->idBroker << " el idBroker " << msg.idBroker;
    Logger::debug(ss.str(), __FILE__);
    return msg;
}

void AtendedorTestersEspeciales::enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_TAREA_ESPECIAL;
    msg.tester = this->idTester;
    msg.idBroker = this->idBroker;
    std::stringstream ss; ss << "Le asigne al msg id Broker " << msg.idBroker << " el idBroker " << this->idBroker;
    Logger::debug(ss.str(), __FILE__);
    msg.idDispositivo = idDispositivo;
    msg.posicionDispositivo = posicionDispositivo;
    msg.value = tarea;

    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar tarea especial " << tarea << " al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTestersEspeciales::registrarTester() {

    // El primer mensaje que se envia es de registro TODO: Ver devolucion!
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_REGISTRAR_TESTER;
    msg.esTesterEspecial = true;
    msg.tester = this->idTester;
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar mensaje de registro de tester especial. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}