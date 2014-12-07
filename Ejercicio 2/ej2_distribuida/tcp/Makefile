CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb

all: client_receptor client_emisor server_emisor server_receptor u_client u_server term

client_receptor:
	$(CC) $(CFLAGS) tcpclient_receptor.cpp -o tcpclient_receptor -DEJEMPLO_TEST

client_emisor:
	$(CC) $(CFLAGS) tcpclient_emisor.cpp -o tcpclient_emisor -DEJEMPLO_TEST

server_emisor:
	$(CC) $(CFLAGS) tcpserver_emisor.cpp -o tcpserver_emisor -DEJEMPLO_TEST

server_receptor:
	$(CC) $(CFLAGS) tcpserver_receptor.cpp -o tcpserver_receptor -DEJEMPLO_TEST

u_client:
	$(CC) $(CFLAGS) utilizador_client.cpp -o utilizador_client

u_server:
	$(CC) $(CFLAGS) utilizador_server.cpp -o utilizador_server

term:
	$(CC) $(CFLAGS) terminador.cpp -o terminador
	
clean:
	rm -f tcpclient_receptor
	rm -f tcpclient_emisor
	rm -f tcpserver_emisor
	rm -f tcpserver_receptor
	rm -f utilizador_client
	rm -f utilizador_server
	rm -f terminador
