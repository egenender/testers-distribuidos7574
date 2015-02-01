#include "AtendedorTesters.h"
#include <cstdlib>

AtendedorTesters::AtendedorTesters(): sem_cola_especiales(SEM_COLA_ESPECIALES) {
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIOS_TESTER);
    this->cola_envios = msgget(key, 0666);
    if(this->cola_requerimiento == -1) {
		std::string err = std::string("Error al obtener la cola de ENVIOS DEL TESTER. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_TESTER);
    this->cola_recepciones = msgget(key, 0666);
    if(this->cola_recepciones == -1) {
        std::string err = std::string("Error al obtener la cola de recepcion del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    key = ftok(ipcFileName.c_str(), MSGQUEUE_TESTERS_ESPECIALES);
    this->cola_testers_especiales = msgget(key, 0666);
    if(this->cola_testers_especiales == -1) {
        std::string err = std::string("Error al obtener la cola para enviar a los testers especiales. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
     }

    sem_cola_especiales.getSem();

    char param_id[10];
    sprintf(param_id, "%d", this->idTester);
    char param_cola[10];
    sprintf(param_cola, "%d", MSGQUEUE_RECEPCIONES_TESTER);
    pid_t receptor = fork();
    if (receptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR_TESTERS,
				param_cola,(char*)0);
        exit(1);
	}
	char param_pid[10];
	sprintf(param_pid, "%d", receptor);
	sprintf(param_cola, "%d", MSGQUEUE_ENVIOS_TESTER);

	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR_TESTERS,
				param_id, param_cola, param_pid,(char*)0);
        exit(1);
	}

}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig): sem_cola_especiales(SEM_COLA_ESPECIALES) {
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recepciones, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
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

int AtendedorTesters::recibirResultado(int idTester) {
	TMessageAtendedor rsp;
    int ret = msgrcv(this->cola_recepciones, &rsp, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return rsp.value;
}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtype_envio = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.idDispositivo = idDispositivo;
    msg.value = orden;
    msg.es_requerimiento = 0;
    
    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarTareaEspecial(int idDispositivo, int idTester, int tarea, int posicionDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = this->idTester; //especial
    msg.mtype_envio = idDispositivo;
    msg.finalizar_conexion = 0;
    msg.idDispositivo = idDispositivo;
    msg.posicionDispositivo = posicionDispositivo;
    msg.value = tarea;
    msg.es_especial = 1;

    int ret = msgsnd(this->cola_envios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar tarea especial " << tarea << " al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(bool cuales[], int idDispositivo, int posicionDispositivo){
	sem_cola_especiales.p();
	for (int i = 0; i < CANT_TESTERS_ESPECIALES; i++){
            if (!cuales[i]) continue;
            TMessageAssignTE msg;
            msg.mtype = i + ID_TESTER_ESPECIAL_START;
            msg.idDispositivo = idDispositivo;
            msg.posicionDispositivo = posicionDispositivo;
            std::stringstream ss;
            ss << "Se envia requerimiento especial a tester especial " << msg.mtype;
            Logger::debug(ss.str(), __FILE__);
            ss.str("");
            int ret = msgsnd(this->cola_testers_especiales, &msg, sizeof(TMessageAssignTE) - sizeof(long), 0);
            if(ret == -1) {
                std::string error = std::string("Error al asignar dispositivos a testers especiales. Error: ") + std::string(strerror(errno));
                Logger::error(error.c_str(), __FILE__);
                exit(0);
            }
	}
	sem_cola_especiales.v();
}

TMessageAssignTE AtendedorTesters::recibirRequerimientoEspecial(int idEsp) {

    TMessageAssignTE msg;
    int ret = msgrcv(this->cola_testers_especiales, &msg, sizeof(TMessageAssignTE) - sizeof(long), idEsp, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
    return msg;
}

bool AtendedorTesters::destruirComunicacion() {
    return (msgctl(this->cola_recepciones, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->cola_recepciones, IPC_RMID, (struct msqid_ds*)0) != -1);
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

	if (status < 0 ){ //Si no me puedo registrar, no puedo laburar (deberia devolver el id, lo dejo en TODO)
		exit(status);
	}
}

void devolverIdTester(int id, int tipo){
	char param_id[5];
	sprintf(param_id, "%d", id);
	char param_tipo[3];
	sprintf(param_tipo, "%d", tipo);

	if (fork() == 0){
		execlp("./broker/servicio_rpc/devolver_id", "devolver_id", UBICACION_SERVER ,param_tipo, param_id,(char*)0);
		printf("ALGO NO ANDUVO\n");
        exit(1);
	}

	wait(NULL);
}
