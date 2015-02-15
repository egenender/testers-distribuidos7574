#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]){
  pid_t pidPadre = atoi(argv[1]);
  char mostrar[300];
  sprintf(mostrar,"[TIMEOUT] --> Lanzado para %d\t| pid: %d\n",pidPadre,getpid());
  write(fileno(stdout),mostrar,strlen(mostrar));
  sleep(10);
  sprintf(mostrar,"[TIMEOUT] --> Expiro");
  write(fileno(stdout),mostrar,strlen(mostrar));
  kill(pidPadre,SIGUSR1);
  return 0;
}
