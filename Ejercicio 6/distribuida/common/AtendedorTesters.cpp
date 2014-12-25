#include "AtendedorTesters.h"
#include <cstdlib>
#include <sys/wait.h>

int getIdTester(int);
void activarTester(int);

AtendedorTesters::AtendedorTesters(int tipo): sem_cola_especiales(SEM_COLA_ESPECIALES) {
	this->idTester = getIdTester(tipo);
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
    sprintf(param_id, "%d", this->idTester);
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
	activarTester(this->idTester);
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtype_envio = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.tester = this->idTester;
    msg.value = idPrograma;
    msg.es_requerimiento = 0;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

int AtendedorTesters::recibirResultado() {
    TMessageAtendedor msg;
    
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
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
	msg.mtype_envio = MTYPE_REQUERIMIENTO; //ID_BROKER!!
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

int AtendedorTesters::recibirRequerimientoEspecial() {
	activarTester(this->idTester);
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg.value;
}

void AtendedorTesters::terminar_atencion(){
	TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.finalizar_conexion = FINALIZAR_CONEXION;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}


int AtendedorTesters::obtenerIdTester(){
	return this->idTester;
}


int getIdTester(int tipo){
	if( tipo != TIPO_COMUN && tipo != TIPO_ESPECIAL) exit(1);
	
	key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_GETTING_IDS);
    int cola_ids = msgget(key, 0666 | IPC_CREAT);
    
    char param_tipo[3];
    sprintf(param_tipo, "%d",tipo);
    
	if (fork() == 0){
		execlp("./broker/servicio_rpc/get_id", "get_id", UBICACION_SERVER ,param_tipo,(char*)0);
		printf("ALGO NO ANDUVO\n");
        exit(1);
	}
		
	wait(NULL);
	
	TMessageAtendedor msg;
    int ret = msgrcv(cola_ids, &msg, sizeof(TMessageAtendedor) - sizeof(long), tipo + 1, 0);
    if (ret == -1){
		//No se pudo conseguir id
		exit(-1);
	}
	
	int id = msg.value;

	if (id <= 0){
		//Algo salio mal, no quedan ids, o lo que fuere
		exit(id);
	}
	return id;
}

void activarTester(int id){
	char param_id[5];
    sprintf(param_id, "%d",id);
    
	if (fork() == 0){
		execlp("./broker/servicio_rpc/registrar_tester", "get_registrar_tester", UBICACION_SERVER ,param_id,(char*)0);
		printf("ALGO NO ANDUVO\n");
        exit(1);
	}
	int status;	
	wait(&status);
	
	//printf("resultado de registracion: %d\n", status);
	if (status < 0 ){ //Si no me puedo registrar, no puedo laburar (deberia devolver el id, lo dejo en TODO)
		exit(status);
	}
	//printf("REGISTRACION EXITOSA de tester %d\n", id);
}
