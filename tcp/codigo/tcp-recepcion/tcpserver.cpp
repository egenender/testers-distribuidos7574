#include "../tcp.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>
#include <sys/msg.h>

int main(int argc, char *argv[])
{

    if(argc != 5){
      printf("Uso: %s <puerto> <id_cola> <tam> <id_lectura>\n", argv[0]);
      return -1;
    }
	
	int id_cola = atoi(argv[2]);
	
    size_t size = atoi(argv[3]);
    
    int id_lectura = atoi(argv[4]);
    
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

    printf("Esperando conextiones...\n");
    
    key_t key = ftok("ipcs-prueba", id_cola);
    int cola = msgget(key, 0660);
	
    while(1){
		int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);
		
		if (fork() == 0){

			int enviado=0, acumulado=0;
      
			void* buffer = malloc(size);
			int ok_read = msgrcv(cola, buffer, size - sizeof(long), id_lectura, 0);
			if (ok_read == -1){
				exit(0);
			}
			printf("Enviando %zu bytes...\n", size);
		
			while((enviado = write(clientfd, buffer, size)) > 0 && acumulado < size)
				acumulado += enviado;

			printf("Enviados %d bytes\n", acumulado);
      
			free(buffer);
			close(clientfd);
			exit(0);
			
		}
		close(clientfd);
	}
	
}
