#include "../tcp.h"
#include <signal.h>
#include <string.h>

/**
 * Servidor tcp que envia a cada cliente un mensaje (basura) del tamaño indicado en bytes.
 */
int main(int argc, char *argv[])
{
    char buffer[] = "Lorem ipsum";

    if(argc != 2){
      printf("Uso: %s <puerto>\n", argv[0]);
      return -1;
    }

    double size = strlen(buffer);
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

    while(1){
      int clientfd = accept(fd, (struct sockaddr*)NULL, NULL);

      double enviado=0, acumulado=0;

      printf("Enviando %i bytes...\n", size);

      while((enviado = write(clientfd, buffer, strlen(buffer))) > 0 && acumulado < size)
	acumulado += enviado;

      printf("Enviados %i bytes\n", acumulado);
      
      close(clientfd);
    }

}
