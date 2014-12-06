#include "AtendedorTesters.h"
#include "Configuracion.h"

AtendedorTesters::AtendedorTesters( const Configuracion& config ) {
    const std::string archivoIpcs = config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS);
    key_t key;
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_NUEVO_REQUERIMIENTO) );
    this->cola_requerimiento = msgget(key, 0666 | IPC_CREAT);
    
    std::stringstream ss; //<DBG>
    ss << "MSGQUEUE_NUEVO_REQUERIMIENTO creada con id " << cola_requerimiento;
    Logger::notice( ss.str().c_str(), __FILE__ );
    ss.str("");
    
    if(this->cola_requerimiento == -1) {
        std::string err = std::string("Error al obtener la cola de requerimientos del atendedor de testers. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
    
    key = ftok( archivoIpcs.c_str(),
                config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_LECTURA_RESULTADOS) );
    this->cola_recibos_tests = msgget(key, 0666 | IPC_CREAT);
    
    ss << "MSGQUEUE_RESULTADOS creada con id " << cola_recibos_tests;
    Logger::notice( ss.str().c_str(), __FILE__ );
    ss.str("");
    
    if(this->cola_recibos_tests == -1) {
        msgctl(this->cola_requerimiento, IPC_RMID, (struct msqid_ds*)0); //TODO Esto no le incumbe, es cosa del finalizador
        std::string err = std::string("Error al obtener la cola del atendedor de testers. Errno: ") + std::string(strerror(errno));
        throw std::string(err.c_str());
    }
    
}

AtendedorTesters::~AtendedorTesters() {
}

int AtendedorTesters::recibirRequerimiento() {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), Constantes::MTYPE_REQUERIMIENTO, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

int AtendedorTesters::recibir2doRequerimiento(int idTester) {

    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_recibos_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir requerimiento del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.idDispositivo;
}

void AtendedorTesters::enviarPrograma(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.idTester = tester; 
    msg.programa = idPrograma;
    
    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar programa al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    
    Logger::notice("Se envio el programa desde atendedor testers", __FILE__);

}

resultado_test_t AtendedorTesters::recibirResultado(int idTester) {

    resultado_test_t rsp;
    TMessageAtendedor msg;

    std::stringstream ss;
    ss << "Intenta recibir una respuesta con mtype: " << idTester << " de la cola: " << this->cola_recibos_tests;
    Logger::notice(ss.str().c_str(), __FILE__);
    ss.str("");
    
    int ret = msgrcv(this->cola_recibos_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), idTester, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    
    Logger::notice("SACO LA RESPUESTA DE LA COLA DE RESPUESTAS", __FILE__);
    rsp.tester = msg.mtype;
    rsp.dispositivo = msg.idDispositivo;
    rsp.result = msg.resultado;
    
     
    return rsp;

}

void AtendedorTesters::enviarOrden(int idDispositivo, int orden) {
    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = idDispositivo;
    msg.orden = orden;
    
    int ret = msgsnd(this->cola_recibos_tests, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar orden al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

}

bool AtendedorTesters::destruirComunicacion() {

    bool colaRequerimientosBorrada = msgctl(this->cola_requerimiento, IPC_RMID,     (struct msqid_ds*)0) != -1;
    if( !colaRequerimientosBorrada ){
        std::string error = std::string("Error al eliminar cola de requerimientos. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
    }
    bool colaRecibosBorrada = msgctl(this->cola_recibos_tests, IPC_RMID, (struct msqid_ds*)0) != -1;
    if( !colaRecibosBorrada ){
        std::string error = std::string("Error al eliminar cola de recibos. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
    }
    return (colaRequerimientosBorrada && colaRecibosBorrada);
}

