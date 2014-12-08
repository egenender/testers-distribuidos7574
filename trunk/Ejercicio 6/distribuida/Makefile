CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG -DVERSION_DISTRIBUIDA

all: dispositivo tecnico testerComun testerEspecial server_emisor server_receptor cliente_emisor cliente_receptor iniciador terminador cleanO

lockfile:
	$(CC) $(CFLAGS) -c sync/LockFile.cpp
	
semaphore:
	$(CC) $(CFLAGS) -c ipc/Semaphore.cpp

atendedorDisp:
	$(CC) $(CFLAGS) -c common/AtendedorDispositivos.cpp
	
atendedorTest:
	$(CC) $(CFLAGS) -c common/AtendedorTesters.cpp
	
despachadorTec:
	$(CC) $(CFLAGS) -c common/DespachadorTecnicos.cpp
	
despachadorTester:
	$(CC) $(CFLAGS) -c common/DespachadorTesters.cpp

logger:
	$(CC) $(CFLAGS) -c logger/Logger.cpp

dispositivo: lockfile logger atendedorDisp
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o AtendedorDispositivos.o Dispositivo.cpp -o Dispositivo

tecnico: lockfile logger despachadorTec
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o DespachadorTecnicos.o Tecnico.cpp -o Tecnico

resultado:
	$(CC) $(CFLAGS) -c common/Resultado.cpp
	
programa:
	$(CC) $(CFLAGS) -c common/Programa.cpp	
	
testerComun: programa resultado lockfile logger despachadorTester atendedorTest planilla
	$(CC) $(CFLAGS) Programa.o Resultado.o Semaphore.o LockFile.o logger/Logger.o AtendedorTesters.o DespachadorTesters.o TesterComun.cpp Planilla.o -o TesterComun
	
testerEspecial: programa resultado lockfile logger despachadorTester atendedorTest planilla
	$(CC) $(CFLAGS) Programa.o Resultado.o Semaphore.o LockFile.o logger/Logger.o DespachadorTesters.o AtendedorTesters.o TesterEspecial.cpp Planilla.o -o TesterEspecial

planilla: semaphore logger
	$(CC) $(CFLAGS) -c common/Planilla.cpp	

cliente_receptor:
	$(CC) $(CFLAGS) tcp/tcpclient_receptor.cpp -o tcp/tcpclient_receptor 
	
cliente_emisor:
	$(CC) $(CFLAGS) tcp/tcpclient_emisor.cpp -o tcp/tcpclient_emisor
	
server_receptor:
	$(CC) $(CFLAGS) tcp/tcpserver_receptor.cpp -o tcp/tcpserver_receptor 
	
server_emisor:
	$(CC) $(CFLAGS) tcp/tcpserver_emisor.cpp -o tcp/tcpserver_emisor

iniciador: semaphore logger lockfile
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o iniciador.cpp -o iniciador

terminador: semaphore lockfile logger
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o terminador.cpp -o terminador
cleanO:
	rm -f *.o

clean: cleanO
	rm -f log.txt
	rm -f Dispositivo
	rm -f Tecnico
	rm -f TesterComun
	rm -f TesterEspecial
	rm -f tcp/tcpclient_receptor
	rm -f tcp/tcpclient_emisor
	rm -f tcp/tcpserver_receptor
	rm -f tcp/tcpserver_emisor
	rm -f iniciador
	rm -f terminador
