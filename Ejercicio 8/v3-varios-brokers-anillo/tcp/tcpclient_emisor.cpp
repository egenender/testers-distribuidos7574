#include "tcp.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "common/common.h"
#include "comunes_tcp.h"
#include "logger/Logger.h"
/*
#ifdef EJEMPLO_TEST
#define IPCS_FILE "ipcs-prueba"
#else
#define IPCS_FILE "/tmp/pereira-ipcs"
#endif
 */
int main(int argc, char *argv[]){
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    if(argc != 5) {
        Logger::error("Bad arguments", __FILE__);
        printf("%s <host> <port> <identificador> <idMsgQueue>",argv[0]);
        return -1;
    }

    int id = atoi(argv[3]); // Id de Disp, o de Tester o de EqEsp, dependiendo de quien lo llame
    int idMsgQueue = atoi(argv[4]);

    int fd = tcpOpenActivo(argv[1], atoi(argv[2]));
    if(fd < 0) {
        Logger::error("Could not create socket", __FILE__);
        return -2;
    }
    
    key_t key = ftok(ipcFileName.c_str(), idMsgQueue);
    int msgQueue = msgget(key, 0660);
      
    /* FIN del setup */
    
    // Envio primer mensaje, para pasarle mi id al servidor
    TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(sizeof(TMessageAtendedor));
    /*buffer->mtype = 1;
    buffer->idDispositivo = id;
    enviar(buffer, fd);*/
    size_t size = sizeof(TMessageAtendedor);
    while (true) {
        //Espero mensaje de la cola
        int okRead = msgrcv(msgQueue, buffer, size - sizeof(long), id, 0);
        if (okRead == -1) {
            Logger::error("Error de lectura de la cola de mensajes", __FILE__);
            close(fd);
            exit(1);
        }
        //Si era un mensaje de finalizacion, envio info para matar al receptor
        // y continuo con mi labor
        /*if (ok_read == -1 || buffer->finalizar_conexion) {
            buffer->mtype = buffer->mtype_envio;
            enviar(buffer, fd);

            //kill(receptor, SIGHUP);
            close(fd);
            free(buffer);
            exit(0);
        }*/

        buffer->mtype = buffer->mtypeMensaje;
        enviar(fd, buffer, size);
    }
    
    close(fd);
    return 0;
}
