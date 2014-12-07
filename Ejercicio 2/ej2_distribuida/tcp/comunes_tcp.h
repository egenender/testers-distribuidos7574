#include "../common/common.h"
#include <stdlib.h>
#include <stddef.h>

void recibir (TMessageAtendedor* buffer, int fd){
	size_t size = sizeof(TMessageAtendedor);
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
		perror("Error recibiendo mensaje desde el servidor");
		exit(1);
	}
}

void enviar(TMessageAtendedor* buffer, int fd){
	size_t acumulado = 0;
	size_t size = sizeof(TMessageAtendedor);
	int enviado = 0;
	char* buffer_envio = (char*) buffer;			
	while((enviado = write(fd, buffer_envio + acumulado, size - acumulado)) >= 0 && acumulado < size){
		acumulado += enviado;
	}
	if (acumulado != size){
		perror("Error al enviar el mensaje al cliente");
		exit(1);
	}
}
