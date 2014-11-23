#include "../tcp.h"
#include "../measure.h"
#include <errno.h>

/**
 * Cliente que recibe datos del socket leyendo cada vez la cantidad de bytes indicada.
 */
int main(int argc, char *argv[])
{
    char buffer[1024];

    if(argc != 6){
        printf("%s <host> <port> <tamaño de recepción inicial> <tamaño de recepción final> <pasos>\n",argv[0]);
        return -1;
    }

    int size_0 = atoi(argv[3]);
    int size_1 = atoi(argv[4]);
    int steps = atoi(argv[5]);
    
    int fd = tcp_open_activo(argv[1], atoi(argv[2]));
    if(fd<0){
      perror("Error");
      return -2;
    }
    
    int leido=0, acumulado=0, lecturas=0;
    int size = size_0;
    int size_step = (size_1 - size_0) / steps;
    int i=0;

    printf("Conectado al servidor. Presione una tecla para continuar.\n");
    getchar();

    tic();
    do{
        leido = read(fd, buffer, size);
        lecturas++;
        acumulado += leido;
        size += size_step;
        if(++i >= steps){
            i=0;
            size = size_0;
        }
    }while( leido > 0);

    double time_ms=toc();

    printf("Tiempo total: %f ms.\n", time_ms);
    
    printf("Recibidos %i bytes en %i lecturas\n", acumulado, lecturas);

    return 0;
}
