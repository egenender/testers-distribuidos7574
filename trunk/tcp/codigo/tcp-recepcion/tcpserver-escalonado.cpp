#include "../tcp.h"
#include <signal.h>

/**
 * Servidor tcp que envia a cada cliente un mensaje (basura) del tamaño indicado en bytes.
 */
int main(int argc, char *argv[])
{
    char buffer[1025];

    if(argc != 6){
      printf("Uso: %s <puerto> <tamaño a enviar en MB> <tamaño inicial del mensaje en bytes> <tamaño final del mensaje en bytes> <pasos>\n", argv[0]);
      return -1;
    }

    double total_size = atoi(argv[2])*1024.0*1024.0;
    int size_0 = atoi(argv[3]);
    int size_1 = atoi(argv[4]);
    int steps = atoi(argv[5]);

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

      double enviado=0.0, acumulado=0.0;
      int escrituras = 0;
      int size = size_0;
      int size_step = (size_1 - size_0) / steps;
      int i=0;

      printf("Enviando %i bytes...\n", total_size);

      do{
          enviado = write(clientfd, buffer, size);
          escrituras++;
          acumulado += enviado;
          size += size_step;
          if(++i >= steps){
              i=0;
              size = size_0;
          }
      }while(enviado > 0 && acumulado < total_size);

      printf("Enviados %d bytes\n", acumulado);
      
      close(clientfd);
    }

}
