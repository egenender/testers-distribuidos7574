#include "AtendedorTesters.h"

static AtendedorTesters* atendedorTester;
static pid_t emisor;
static pid_t receptor;
static int testerId;

int desregistrarTesterComun(int id);

void restartTester(int sigNum) {

    // Debo killear al emisor y receptor, lanzar otro proceso dispositivo y killearme
    kill(emisor, SIGQUIT);
    kill(receptor, SIGQUIT);
    // Por las dudas, desregistro el tester
    atendedorTester->desregistrarTester(testerId);
    desregistrarTesterComun(testerId);
    if (fork() == 0) {
        // Inicio el programa correspondiente
        execlp("./testerComun", "testerComun", (char*)0);
        Logger::error("Error al reiniciar el programa TesterComun", __FILE__);
        exit(1);
    }
    exit(1);
}

AtendedorTesters::AtendedorTesters() {}

AtendedorTesters::AtendedorTesters(int idTester): idTester(idTester) {
    
    testerId = this->idTester;
    atendedorTester = this;

    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTER_COMUN);
    this->colaEnvios = msgget(key, 0666);
    if(this->colaEnvios == -1) {
		std::string err = std::string("Error al obtener la cola de ENVIOS DEL TESTER. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_NO_REQ_TESTER_COMUN);
    this->colaRecepcionesGeneral = msgget(key, 0666);
    if(this->colaRecepcionesGeneral == -1) {
        std::string err = std::string("Error al obtener la cola de recepcion de mensajes generales del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_REQUERIMIENTOS_TESTER_COMUN);
    this->colaRecepcionesRequerimientos = msgget(key, 0666);
    if(this->colaRecepcionesRequerimientos == -1) {
        std::string err = std::string("Error al obtener la cola de recepcion de requerimientos del tester. Errno: ") + std::string(strerror(errno));
        Logger::error(err, __FILE__);
        exit(1);
    }

    char paramSizeMsg[10];
    char paramId[10];
    sprintf(paramId, "%d", this->idTester);
    char paramCola[10];
    sprintf(paramCola, "%d", MSGQUEUE_RECEPCIONES_TESTER_COMUN);
    sprintf(paramSizeMsg, "%d", (int) sizeof(TMessageAtendedor));
    this->pidReceptor = fork();
    if (this->pidReceptor == 0){
		execlp("./tcp/tcpclient_receptor", "tcpclient_receptor",
				UBICACION_SERVER,
				PUERTO_SERVER_EMISOR,
				paramId, paramCola, paramSizeMsg, 
                (char*)0);
        exit(1);
	}

	sprintf(paramCola, "%d", MSGQUEUE_ENVIO_TESTER_COMUN);

	if (fork() == 0){
		execlp("./tcp/tcpclient_emisor", "tcpclient_emisor",
				UBICACION_SERVER,
				PUERTO_SERVER_RECEPTOR,
				paramId, paramCola, paramSizeMsg, (char*)0);
        exit(1);
	}
    
    registrarTester();
    
    // Determino el handler de la señal en caso de timeout
    struct sigaction action;
    action.sa_handler = restartTester;
    int sigOk = sigaction(SIGUSR1, &action, 0);
    if (sigOk == -1) {
        Logger::error("Error al setear el handler de la señal!", __FILE__);
        exit(1);
    }

    emisor = this->pidEmisor;
    receptor = this->pidReceptor;
}

AtendedorTesters::AtendedorTesters(const AtendedorTesters& orig) {}

AtendedorTesters::~AtendedorTesters() {
    kill(this->pidReceptor, SIGINT);
    kill(this->pidEmisor, SIGINT);
    int status;
    waitpid(this->pidReceptor, &status, 0);
    waitpid(this->pidEmisor, &status, 0);
}

int AtendedorTesters::recibirRequerimiento() {
    
    TMessageAtendedor msg;
    int ret = msgrcv(this->colaRecepcionesRequerimientos, &msg, sizeof(TMessageAtendedor) - sizeof(long), this->idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

    this->idBroker = msg.idBrokerOrigen;
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_PROGRAMA_INICIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = this->idTester;
    msg.idBroker = this->idBroker;
    msg.idBrokerOrigen = ID_BROKER;
    msg.value = idPrograma;
    
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

TMessageAtendedor AtendedorTesters::recibirResultado(int idTester) {
    
    Timeout timeout;
    timeout.runTimeout(SLEEP_TIMEOUT_TESTERS, getpid(), SIGUSR1);

	TMessageAtendedor rsp;
    int ret = msgrcv(this->colaRecepcionesGeneral, &rsp, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

    timeout.killTimeout();

    this->idBroker = rsp.idBrokerOrigen;
    return rsp;
}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_ORDEN;
    msg.tester = this->idTester;
    msg.idDispositivo = idDispositivo;
    msg.idBroker = this->idBroker;
    msg.idBrokerOrigen = ID_BROKER;
    msg.value = orden;
    
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}
void AtendedorTesters::enviarReqTestConfig( int idDispositivo, int idTester, int tipoDispositivo ){
    TMessageTesterConfig msg;
    msg.mtype = idTester;
    msg.idDispositivo = idDispositivo;
    msg.tipoDispositivo = tipoDispositivo;
    int ret = msgsnd(m_ColaTestersConfig, &msg, sizeof(TMessageTesterConfig) - sizeof(long), 0);
    if(ret == -1) {
        std::stringstream ss;
        ss << "Error al enviar test config al dispositivo " << idDispositivo << " desde el tester " << idTester << ". Error: ";
        std::string error = ss.str() + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::enviarAEspeciales(bool cuales[], int idDispositivo, int posicionDispositivo){
    int j = 0;
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_REQUERIMIENTO_TESTER_ESPECIAL;
    msg.idDispositivo = idDispositivo;
    msg.tester = this->idTester;
    msg.idBrokerOrigen = this->idBroker; // Le asigno el broker del dispositivo
    msg.posicionDispositivo = posicionDispositivo;
    msg.cantTestersEspecialesAsignados = 0;
	for (int i = 0; (i < CANT_TESTERS_ESPECIALES) && (j < MAX_TESTERS_ESPECIALES_PARA_ASIGNAR); i++) {
        if (!cuales[i]) continue;
        msg.idTestersEspeciales[j] = i + ID_TESTER_ESP_START; j++;
        msg.cantTestersEspecialesAsignados++;
    }

    std::stringstream ss;
    ss << "Se envian requerimientos especiales a testers especiales: " << msg.idTestersEspeciales[0] << ", " << msg.idTestersEspeciales[1] << ", " << msg.idTestersEspeciales[2] << ", " << msg.idTestersEspeciales[3];
    Logger::debug(ss.str(), __FILE__);
    ss.str("");
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al asignar dispositivo a testers especiales. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

bool AtendedorTesters::destruirComunicacion() {
    return (msgctl(this->colaEnvios, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->colaRecepcionesGeneral, IPC_RMID, (struct msqid_ds*)0) != -1 && msgctl(this->colaRecepcionesRequerimientos, IPC_RMID, (struct msqid_ds*)0) != -1);
}

void AtendedorTesters::registrarTester() {

    // El primer mensaje que se envia es de registro TODO: Ver devolucion!
    TMessageAtendedor msg;
    msg.mtype = this->idTester;
    msg.mtypeMensaje = MTYPE_REGISTRAR_TESTER;
    msg.esTesterEspecial = false;
    msg.tester = this->idTester;
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar mensaje de registro de tester comun. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

void AtendedorTesters::desregistrarTester(int tester) {

    // El primer mensaje que se envia es de registro TODO: Ver devolucion!
    TMessageAtendedor msg;
    msg.mtype = tester;
    msg.mtypeMensaje = MTYPE_DESREGISTRAR_TESTER;
    msg.esTesterEspecial = false;
    msg.tester = tester;
    int ret = msgsnd(this->colaEnvios, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar mensaje de desregistro de tester comun. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }
}

int desregistrarTesterComun(int id) {

    CLIENT *clnt;
    int  *result_4;
    int  devolveridtestercomun_1_arg = id;

    clnt = clnt_create (UBICACION_SERVER_IDENTIFICADOR, IDENTIFICADORPROG, IDENTIFICADORVERS, "udp");
    if (clnt == NULL) {
        Logger::error("Error en la creación del cliente RPC", __FILE__);
        clnt_pcreateerror (UBICACION_SERVER_IDENTIFICADOR);
        exit (1);
    }
    
    result_4 = devolveridtestercomun_1(&devolveridtestercomun_1_arg, clnt);
    if (result_4 == (int *) NULL) {
        Logger::error("Error en la llamada al RPC devolviendo el ID", __FILE__);
        clnt_perror (clnt, "call failed");
    }

    clnt_destroy (clnt);
    
    return *result_4;
}