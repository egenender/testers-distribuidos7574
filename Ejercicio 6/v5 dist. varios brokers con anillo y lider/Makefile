CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb

all: inicia_comp dispositivo tecnico testerComun testerEspecial tcp broker rpc_services ring iniciador lanzaDisp terminador destroy cleanO
	@echo "Compilacion terminada"

inicia_comp:
	@echo "Iniciando compilacion"

lockfile:
	$(CC) $(CFLAGS) -c sync/LockFile.cpp
	
semaphore:
	$(CC) $(CFLAGS) -c ipc/Semaphore.cpp

distributed_semaphore:
	$(CC) $(CFLAGS) -c ipc/semaforos_distribuidos.cpp

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

tcp: cliente_receptor cliente_emisor server_receptor server_emisor

broker: semaphore distributed_semaphore lockfile logger
	$(CC) $(CFLAGS) broker/Broker.cpp LockFile.o logger/Logger.o Semaphore.o -o Broker
	$(CC) $(CFLAGS) broker/broker_pasa_manos.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_pasa_manos
	$(CC) $(CFLAGS) broker/broker_req_especiales.cpp LockFile.o logger/Logger.o Semaphore.o semaforos_distribuidos.o -o broker/broker_req_especiales
	$(CC) $(CFLAGS) broker/broker_requerimientos.cpp LockFile.o logger/Logger.o Semaphore.o semaforos_distribuidos.o -o broker/broker_requerimientos
	$(CC) $(CFLAGS) broker/broker_shm_testers_req.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_shm_testers_req
	$(CC) $(CFLAGS) broker/broker_shm_testers_otorga.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_shm_testers_otorga
	$(CC) $(CFLAGS) broker/broker_disponibilidad_testers.cpp LockFile.o logger/Logger.o Semaphore.o semaforos_distribuidos.o -o broker/broker_disponibilidad_testers
	$(CC) $(CFLAGS) broker/broker_cola_shm.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_cola_shm
	$(CC) $(CFLAGS) broker/broker_pasa_brokers_testers.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_pasa_brokers_testers
	$(CC) $(CFLAGS) broker/broker_pasa_brokers_dispositivos.cpp LockFile.o logger/Logger.o Semaphore.o -o broker/broker_pasa_brokers_dispositivos

rpc_services:
	@make -f servicio_rpc/Makefile.registracion
	
ring:
	gcc  Anillo/sender.c -o Anillo/sender 
	gcc  Anillo/listener.c -o Anillo/listener 
	gcc  Anillo/timeout.c -o Anillo/timeout

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
	@echo "killall tcpclient_receptor" >> destroy.sh
	@echo "killall tcpserver_emisor" >> destroy.sh
	@echo "killall registracion_server" >> destroy.sh
	@chmod +x destroy.sh

cleanO:
	@echo "Eliminando .o's"
	@rm -f *.o
	@rm -f servicio_rpc/*.o

clean: cleanO
	@echo "Eliminando log"
	@rm -f log.txt
	@echo "Eliminando ejecutables"
	@rm -f Dispositivo Tecnico TesterComun TesterEspecial tcp/tcpclient_receptor tcp/tcpclient_emisor tcp/tcpserver_receptor tcp/tcpserver_emisor Broker broker/broker_pasa_manos broker/broker_req_especiales broker/broker_requerimientos broker/broker_shm_testers_req broker/broker_shm_testers_otorga broker/broker_disponibilidad_testers broker/broker_cola_shm broker/broker_pasa_brokers_testers broker/broker_pasa_brokers_dispositivos servicio_rpc/registracion_server servicio_rpc/get_id servicio_rpc/devolver_id servicio_rpc/registrar_tester servicio_rpc/consultar_broker_tester Anillo/sender Anillo/listener Anillo/timeout iniciador LanzaDisp terminador
	@echo "Eliminando script destructor"
	@rm -f destroy.sh
