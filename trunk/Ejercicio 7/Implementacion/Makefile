CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG

all: dispositivo tecnico testerComun testerEspecial iniciador terminador cleanO

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
	rm -f iniciador
	rm -f terminador

probar:
	./terminador
	rm -f log.txt
	./iniciador
