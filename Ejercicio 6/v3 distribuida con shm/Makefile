CC=@g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb

all: inicia_comp dispositivo tecnico testerComun testerEspecial server_emisor server_receptor cliente_emisor cliente_receptor broker rpc_services iniciador lanzaDisp terminador destroy cleanO
	@echo "Compilacion terminada"

inicia_comp:
	@echo "Iniciando compilacion"

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

broker: semaphore lockfile logger
	$(CC) $(CFLAGS) broker/Broker.cpp LockFile.o logger/Logger.o Semaphore.o -o Broker
	$(CC) $(CFLAGS) broker/broker_pasa_manos.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_pasa_manos
	$(CC) $(CFLAGS) broker/broker_req_especiales.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_req_especiales
	$(CC) $(CFLAGS) broker/broker_requerimientos.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_requerimientos

rpc_services:
	@make -f broker/servicio_rpc/Makefile.registracion

iniciador: semaphore logger lockfile
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o iniciador.cpp -o iniciador

lanzaDisp: semaphore logger lockfile
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o LanzaDisp.cpp -o LanzaDisp

terminador: semaphore lockfile logger
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o terminador.cpp -o terminador

destroy:
	@echo "Creando script destructor"
	@echo "./terminador" > destroy.sh
	@echo "killall tcpserver_receptor" >> destroy.sh
	@echo "killall tcpserver_emisor" >> destroy.sh
	@echo "killall registracion_server" >> destroy.sh
	@chmod +x destroy.sh

cleanO:
	@echo "Eliminando .o's"
	@rm -f *.o
	@rm -f broker/servicio_rpc/*.o

clean: cleanO
	@echo "Eliminando log"
	@rm -f log.txt
	@echo "Eliminando ejecutables"
	@rm -f Dispositivo Tecnico TesterComun TesterEspecial tcp/tcpclient_receptor tcp/tcpclient_emisor tcp/tcpserver_receptor tcp/tcpserver_emisor Broker broker/broker_pasa_manos broker/broker_req_especiales broker/broker_requerimientos broker/servicio_rpc/registracion_server broker/servicio_rpc/get_id broker/servicio_rpc/registrar_tester broker/servicio_rpc/devolver_id iniciador LanzaDisp terminador
	@echo "Eliminando script destructor"
	@rm -f destroy.sh
