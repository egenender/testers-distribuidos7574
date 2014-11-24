#include "tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>
#include "../common/common.h"
#include "../logger/Logger.h"

int main(int argc, char *argv[]){
	std::stringstream name;
    name << __FILE__ ;
    Logger::notice("Empiezo a trabajar", name.str().c_str());
    printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");

    /*if(argc != 3){
      printf("Uso: %s <puerto> <id_cola>\n", argv[0]);
      return -1;
    }*/
	
	int id_cola = atoi(argv[2]);
	
    size_t size = sizeof(TMessageAtendedor);
    
    if(size <= 0){
      printf("Error: el tamaño del mensaje debe ser mayor a cero\n");
      return -2;
    }

    int fd = tcp_open_pasivo(atoi(argv[1]));
    if(fd < 0){
      perror("Error");
      return -3;
    }
    
    tcp_disable_nagle(fd);

    if(listen(fd, 10) != 0){
      perror("Error en el listen");
      return -4;
    }

    signal(SIGPIPE, SIG_IGN);
    
    key_t key = ftok(ipcFileName.c_str(), id_cola);
    //key_t key = ftok("ipcs-prueba", id_cola);
    int cola = msgget(key, 0660);
		
    while(1){
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
		
		if (fork() == 0){
			Logger::notice("Acepte un nuevo cliente receptor", name.str().c_str());
			int enviado = 0, acumulado = 0, leido = 0;			
			TMessageAtendedor* buffer = (TMessageAtendedor*) malloc(size);
			Logger::notice("Voy a leer idDispositivo por el socket", name.str().c_str());
    
			char* buffer_act = (char*) buffer;
			while( (leido = read(fd, buffer_act, size - acumulado)) > 0){
				acumulado += leido;
				buffer_act = buffer_act + leido;
			}
			Logger::notice("Termine de leer el idDispositivo desde el socket", name.str().c_str());
			
			long dispositivo_a_tratar = buffer->idDispositivo;
			while (true){
				Logger::notice("Voy a leer a la cola.", name.str().c_str());
				int ok_read = msgrcv(cola, buffer, size - sizeof(long), dispositivo_a_tratar, 0);
				if (ok_read == -1){
					exit(0);
				}
				Logger::notice("Lei de la cola. Me pongo a enviar al socket", name.str().c_str());   
				
				char* buffer_envio = (char*) buffer;			
				while((enviado = write(clientfd, buffer_envio + acumulado, size - acumulado)) > 0 && acumulado < size){
					acumulado += enviado;
				}
				Logger::notice("Termine de enviar mensaje.", name.str().c_str()); 
			}
			free(buffer);
			close(clientfd);
			exit(0);
			
		}
		close(clientfd);
	}
	
}
