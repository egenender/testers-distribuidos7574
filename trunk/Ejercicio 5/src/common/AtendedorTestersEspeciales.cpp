#include "AtendedorTestersEspeciales.h"
#include <cstdlib>

AtendedorTestersEspeciales::AtendedorTestersEspeciales() {
    key_t key;
    key = ftok(ipcFileName.c_str(), MSGQUEUE_ESPECIALES);
    this->cola_pruebasEspeciales = msgget(key, 0666);
    if(this->cola_pruebasEspeciales == -1) {
		std::string err = std::string("Error al obtener la cola de PRUEBAS ESPECIALES del atendedor de testers. Errno: ") + std::string(strerror(errno));
		Logger::error(err, __FILE__);
        exit(1);
    }
}

AtendedorTestersEspeciales::AtendedorTestersEspeciales(const AtendedorTestersEspeciales& orig) {
}

AtendedorTestersEspeciales::~AtendedorTestersEspeciales() {
}

void AtendedorTestersEspeciales::enviarPruebaEspecial(int idDispositivo, int tester, int idPrograma) {

    TMessageAtendedor msg;
    msg.mtype = idDispositivo;
    msg.idDispositivo = tester; //FIXME?
    msg.value = idPrograma;
    
    int ret = msgsnd(this->cola_pruebasEspeciales, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
    if(ret == -1) {
        std::string error = std::string("Error al enviar prueba especial al atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        exit(0);
    }

}

resultado_test_t AtendedorTestersEspeciales::recibirResultadoPruebaEspecial(int idTesterEspecial) {

	resultado_test_t respuesta;
	TMessageAtendedor rsp;
    int ret = msgrcv(this->cola_pruebasEspeciales, &rsp, sizeof(TMessageAtendedor) - sizeof(long), idTesterEspecial, 0);
    if(ret == -1) {
        std::string error = std::string("Error al recibir resultado del atendedor. Error: ") + std::string(strerror(errno));
        Logger::error(error.c_str(), __FILE__);
        return respuesta;
        exit(0);
    }

    respuesta.tester = (long)idTesterEspecial;
    respuesta.result = rsp.value;
    respuesta.dispositivo = rsp.idDispositivo;

    return respuesta;
}

bool AtendedorTestersEspeciales::destruirComunicacion() {

    return (msgctl(this->cola_pruebasEspeciales, IPC_RMID, (struct msqid_ds*)0) != -1 );
}
