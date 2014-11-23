#include "../tcp.h"
#include <errno.h>
#include <stdio.h>
#include "../measure.h"

/**
 * Cliente que recibe datos del socket leyendo cada vez la cantidad de bytes indicada.
 */
int main(int argc, char *argv[])
{
    char buffer[4192];

    if(argc != 4){
        printf("%s <host> <port> <tamaño de recepción>\n",argv[0]);
        return -1;
    }

    int size = atoi(argv[3]);
    if(size<=0 || size>4192){
      printf("Error: el tamaño debe estar entre 1 y 4192\n");
      return -1;
    }
    
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd<0){
      perror("Error");
      return -2;
    }
    
    printf("Conectado al servidor. Presione una tecla para continuar.\n");
    getchar();

    tic();
    int leido=0, acumulado=0, lecturas=0;
    while( (leido = read(fd, buffer, size)) > 0){
        lecturas++;
        acumulado += leido;
        printf("%s\n", buffer);
    }
    double time_ms=toc();

    printf("Tiempo total: %f ms.\n", time_ms);

    printf("Recibidos %i bytes en %i lecturas\n", acumulado, lecturas);

    return 0;
}
