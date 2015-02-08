#include "common/common.h"
#include "logger/Logger.h"
#include <stdlib.h>
#include <stddef.h>

#define MAXIMOS_ATENDIDOS 10

void recibir (int fd, void* buffer, size_t size){
    size_t acumulado = 0;
    int leido = 0;
    char* buffer_act = (char*) buffer;
    // Leo el mensaje en un while, para que si no se recibieron todos los bytes, se sigue leyendo
    while( (leido = read(fd, buffer_act, size - acumulado)) >= 0 && acumulado < size){ //PUEDE QUE HAYA QUE SACAR EL =, PORQUE CUANDO SE CAGA LA CONEXION SE LEEN 0 BYTES....
        acumulado += leido;
        buffer_act = buffer_act + leido;
    }
    // Si hubo algun error, salimos
    if (acumulado != size){
        //perror("Error recibiendo mensaje");
        Logger::error("Error recibiendo mensaje", __FILE__);
        exit(1);
    }
}

void enviar(int fd, void* buffer, size_t size) {
    size_t acumulado = 0;
    int enviado = 0;
    char* buffer_envio = (char*) buffer;			
    while((enviado = write(fd, buffer_envio + acumulado, size - acumulado)) >= 0 && acumulado < size){
        acumulado += enviado;
    }
    if (acumulado != size){
        perror("Error al enviar el mensaje");
        exit(1);
    }
}
