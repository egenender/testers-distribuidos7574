CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG

all: client_receptor server_emisor u_client u_server term

client_receptor:
	make tcpclient_receptor

server_emisor:
	make tcpserver_emisor

u_client:
	make utilizador_client

u_server:
	make utilizador_server

term:
	make terminador
	
clean:
	rm -f tcpclient_receptor
	rm -f tcpserver_emisor
	rm -f utilizador_client
	rm -f utilizador_server
	rm -f terminador
