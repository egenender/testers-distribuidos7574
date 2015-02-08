#include "AtendedorTesters.h"

AtendedorTesters::AtendedorTesters(int idTester): idTester(idTester), semColaEspeciales(SEM_COLA_ESPECIALES) {
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTER_COMUN);
    this->colaEnvios = msgget(key, 0666);
    if(this->colaEnvios == -1) {
		std::string err = std::string("Error al obtener la cola de ENVIOS DEL TESTER. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_TESTER_COMUN);
    this->colaRecepciones = msgget(key, 0666);
    if(this->colaRecepciones == -1) {
        std::string err = std::string("Error al obtener la cola de recepcion del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    semColaEspeciales.getSem();

    char paramId[10];
    sprintf(paramId, "%d", this->idTester);
    char paramCola[10];
    sprintf(paramCola, "%d", MSGQUEUE_RECEPCIONES_TESTER_COMUN);
    pid_t receptor = fork();
    if (receptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR,
				paramId, paramCola,(char*)0);
        exit(1);
	}

	sprintf(paramCola, "%d", MSGQUEUE_ENVIO_TESTER_COMUN);

	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR,
				paramId, paramCola,(char*)0);
        exit(1);
	}

}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig): semColaEspeciales(SEM_COLA_ESPECIALES) {
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_PROGRAMA_INICIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = this->idTester;
    msg.value = idPrograma;
    
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

TMessageAtendedor AtendedorTesters::recibirResultado(int idTester) {
	TMessageAtendedor rsp;
    int ret = msgrcv(this->colaRecepciones, &rsp, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return rsp;
}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_ORDEN;
    msg.tester = this->idTester;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(bool cuales[], int idDispositivo, int posicionDispositivo){
	semColaEspeciales.p();
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
            if (!cuales[i]) continue;
            TMessageAtendedor msg;
            msg.mtype = this->idTester;
            msg.mtypeMensaje = MTYPE_REQUERIMIENTO_TESTER_ESPECIAL;
            msg.idDispositivo = idDispositivo;
            msg.tester = i + ID_TESTER_ESP_START;
            msg.posicionDispositivo = posicionDispositivo;
            std::stringstream ss;
            ss << "Se envian requerimiento especial a tester especial " << msg.tester;
            Logger::debug(ss.str(), __FILE__);
            ss.str("");
            int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
            if(ret == -1) {
                std::string error = std::string("Error al asignar dispositivos a testers especiales. Error: ") + std::string(strerror(errno));
                Logger::error(error.c_str(), __FILE__);
                exit(0);
            }
	}
	semColaEspeciales.v();
}

bool AtendedorTesters::destruirComunicacion() {
    return (msgctl(this->colaRecepciones, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->colaRecepciones, IPC_RMID, (struct msqid_ds*)0) != -1);
}