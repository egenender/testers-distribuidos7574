#ifndef _TCP_H_
#define _TCP_H_
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

const int MAX_BACKLOG = 20;

void tcpDisableNagle(int sock);

/**
 * Abre un socket y lo conecta a un host y puerto.
 */
int tcpOpenActivo(const char* host_name, uint16_t port){
    int fd = 0;
    struct sockaddr_in server;
    struct hostent *host;

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    host = gethostbyname(host_name);
    if (host == NULL)
        return -2;
    
    memset(&server, 0, sizeof(server)); 
    
    server.sin_family = AF_INET;
    server.sin_family = host->h_addrtype;
    memcpy((char *)&(server.sin_addr.s_addr), (host->h_addr_list)[0], host->h_length);
    server.sin_port = htons(port);

    if(connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
        return -3;
    
    return fd;
}

/**
 * Abre un socket y lo deja listo para recibir conexiones entrantes en un puerto.
 */
int tcpOpenPasivo(uint16_t port){
    struct sockaddr_in serv_addr; 
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        // Error!
        return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error en el bind del socket\n");
        return -1;
    }
    
    tcpDisableNagle(fd);
    
    if (listen(fd, MAX_BACKLOG) < 0) {
        perror("Error en el listen del socket\n");
        return -1;
    }

    return fd;
}

void tcpDisableNagle(int sock) {
    int flag = 1;
    /*int result =*/ setsockopt(sock,            /* socket affected */
                            IPPROTO_TCP,     /* set option at TCP level */
                            TCP_NODELAY,     /* name of option */
                            (char *) &flag,  /* the cast is historical cruft */
                            sizeof(int));    /* length of option value */
}

#endif /* _TCP_H_ */
