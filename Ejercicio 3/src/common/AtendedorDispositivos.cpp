#include "AtendedorDispositivos.h"
#include "Configuracion.h"
#include <cstdlib>
#include <cstring>

AtendedorDispositivos::AtendedorDispositivos(const Configuracion& config) {
    const std::string archivoIpcs = config.ObtenerParametroString(Constantes::NombresDeParametros::ARCHIVO_IPCS);
    key_t key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_NUEVO_REQUERIMIENTO));
    this->cola_requerimiento = msgget(key, 0666 | IPC_CREAT);

    std::stringstream ss; //<DBG>

    if (this->cola_requerimiento == -1) {
        throw std::string("Error al obtener la cola del atendedor de dispositivos. Errno: " + errno);
    } else {
        ss << "Cola de requerimientos creada con id " << this->cola_requerimiento;
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");
    }

    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_ESCRITURA_RESULTADOS));
    this->cola_tests = msgget(key, 0666 | IPC_CREAT);


    if (this->cola_tests == -1) {
        msgctl(this->cola_tests, IPC_RMID, (struct msqid_ds*) 0);
        throw std::string("Error al obtener la cola del atendedor de dispositivos. Errno: " + errno);
    } else {
        ss << "Cola de resultados creada con id " << this->cola_tests;
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");
    }

    key = ftok(archivoIpcs.c_str(), config.ObtenerParametroEntero(Constantes::NombresDeParametros::MSGQUEUE_LECTURA_RESULTADOS));
    this->cola_orden = msgget(key, 0666 | IPC_CREAT);

    if (this->cola_orden == -1) {
        msgctl(this->cola_orden, IPC_RMID, (struct msqid_ds*) 0);
        throw std::string("Error al obtener la cola del atendedor de dispositivos. Errno: " + errno);
    } else {
        ss << "Cola de resultados creada con id " << this->cola_orden;
        Logger::notice(ss.str().c_str(), __FILE__);
        ss.str("");
    }
}

AtendedorDispositivos::~AtendedorDispositivos() {
}

void AtendedorDispositivos::enviarRequerimiento(int idDispositivo) {

    TMessageAtendedor msg;
    msg.mtype = Constantes::MTYPE_REQUERIMIENTO;
    msg.idDispositivo = idDispositivo;

    int ret = msgsnd(this->cola_requerimiento, &msg, sizeof (TMessageAtendedor) - sizeof (long), 0);
    if (ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

void AtendedorDispositivos::enviar1erRespuesta(int idDispositivo, int resultado) {

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        exit(-1);
    }

    long nextTesterIdOffset = config.ObtenerParametroEntero("Tester2doIdOffset") - config.ObtenerParametroEntero("TesterIdOffset");

    TMessageAtendedor msg;
    msg.mtype = this->ultimoTester + nextTesterIdOffset;
    msg.idDispositivo = idDispositivo;
    msg.resultado = resultado;
    msg.tipoResultado = PARCIAL;


    int ret = msgsnd(this->cola_tests, &msg, sizeof (TMessageAtendedor) - sizeof (long), 0);
    if (ret == -1) {
        std::string error("Error al enviar mensaje al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
}

int AtendedorDispositivos::recibirPrograma(int idDispositivo) {
    TMessageAtendedor msg;
    int ret = msgrcv(this->cola_requerimiento, &msg, sizeof (TMessageAtendedor) - sizeof (long), idDispositivo, 0);
    if (ret == -1) {
        std::string error("Error al recibir programa del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }

    this->ultimoTester = msg.idTester;
    return msg.programa;
}

void AtendedorDispositivos::enviarResultado(int idDispositivo, int resultado) {

    Configuracion config;
    if (!config.LeerDeArchivo()) {
        Logger::error("Archivo de configuracion no encontrado", __FILE__);
        exit(-1);
    }

    long nextTesterIdOffset = config.ObtenerParametroEntero("TesterRtaIdOffset") - config.ObtenerParametroEntero("Tester2doIdOffset");

    TMessageAtendedor msg;
    msg.mtype = this->ultimoTester + nextTesterIdOffset;
    msg.idDispositivo = idDispositivo;
    msg.resultado = resultado;
    msg.tipoResultado = FINAL;

    int ret = msgsnd(this->cola_tests, &msg, sizeof (TMessageAtendedor) - sizeof (long), 0);
    if (ret == -1) {
        std::string error("Error al enviar resultado al atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    
}

int AtendedorDispositivos::recibirOrden(int idDispositivo) {

    TMessageAtendedor msg;
        
    int ret = msgrcv(this->cola_orden, &msg, sizeof (TMessageAtendedor) - sizeof (long), idDispositivo, 0);
    if (ret == -1) {
        std::string error("Error al recibir orden del atendedor. Error: " + errno);
        Logger::error(error.c_str(), __FILE__);
        throw error;
    }
    return msg.orden;
}


bool AtendedorDispositivos::destruirComunicacion() {

    bool colaRequerimientosBorrada = msgctl(this->cola_requerimiento, IPC_RMID,     (struct msqid_ds*)0) != -1;
    if( !colaRequerimientosBorrada ){
        std::string error = std::string("Error al eliminar cola de requerimientos. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
    }
    bool colaRecibosBorrada = msgctl(this->cola_tests, IPC_RMID, (struct msqid_ds*)0) != -1;
    if( !colaRecibosBorrada ){
        std::string error = std::string("Error al eliminar cola de tests. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
    }
    bool colaOrdenesBorrada = msgctl(this->cola_orden, IPC_RMID, (struct msqid_ds*)0) != -1;
    if( !colaOrdenesBorrada ){
        std::string error = std::string("Error al eliminar cola de ordenes. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
    }
    return (colaRequerimientosBorrada && colaRecibosBorrada && colaOrdenesBorrada);
}
