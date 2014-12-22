#include "AtendedorTesters.h"
#include <cstdlib>

AtendedorTesters::AtendedorTesters(int idTester): sem_cola_especiales(SEM_COLA_ESPECIALES) {
	this->idTester = idTester;
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_RECIBOS);
    this->cola_recibos = msgget(key, 0666);
    if(this->cola_recibos == -1) {
		std::string err = std::string("Error al obtener la cola de requerimientos del atendedor de testers. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_ENVIOS);
    this->cola_envios = msgget(key, 0666);
    if(this->cola_envios == -1) {
        std::string err = std::string("Error al obtener la cola de lectura de resultados del atendedor de testers. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
  
    sem_cola_especiales.getSem();
    
    char param_id[10];
    sprintf(param_id, "%d", idTester);
    char param_cola[10];
    sprintf(param_cola, "%d", MSGQUEUE_TESTERS_RECIBOS);
    pid_t receptor = fork();
    if (receptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",UBICACION_SERVER ,PUERTO_SERVER_EMISOR_TESTERS , param_cola,(char*)0);
        exit(1);
	}
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	sprintf(param_cola, "%d", MSGQUEUE_TESTERS_ENVIOS);
	
	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",UBICACION_SERVER ,PUERTO_SERVER_RECEPTOR_TESTERS , param_id, param_cola, param_pid,(char*)0);
        exit(1);
	}
	
}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig): sem_cola_especiales(SEM_COLA_ESPECIALES) {
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_REQUERIMIENTO, 0);
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
    msg.mtype_envio = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.tester = tester;
    msg.value = idPrograma;
    msg.es_requerimiento = 0;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorTesters::recibirResultado(int idTester) {
    TMessageAtendedor msg;
    
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
        
    return msg.value;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden, int cantidad) {
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtype_envio = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    msg.cant_testers = cantidad;
    msg.es_requerimiento = 0;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(bool cuales[], int posicion){
	//sem_cola_especiales.p();
	TMessageAtendedor msg;
	msg.mtype = this->idTester;
	msg.mtype_envio = 1; //ID_BROKER!!
	msg.value = posicion;
	msg.es_requerimiento = 1;
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
		msg.especiales[i] = cuales[i];
	}
	for (int i = CANT_TESTERS_ESPECIALES; i < MAX_TESTERS_ESPECIALES; i++){
		msg.especiales[i] = false;
	}	
	int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
	if(ret == -1) {
		std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
		Logger::error(error.c_str(), __FILE__);
		exit(0);
	}
	
	//sem_cola_especiales.v();
}

int AtendedorTesters::recibirRequerimientoEspecial(int idEsp) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), idEsp, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;
}

void AtendedorTesters::terminar_atencion(int idDispositivo_atendido){
	TMessageAtendedor msg;
    msg.mtype = idDispositivo_atendido;
    msg.finalizar_conexion = FINALIZAR_CONEXION;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
