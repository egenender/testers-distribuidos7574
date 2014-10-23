CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG

all: dispositivo tecnico arriboResul testerA testerB planillaA planillaB iniciador terminador cleanO

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

arriboResul: semaphore
	$(CC) $(CFLAGS) Semaphore.o ArriboDeResultados.cpp -o ArriboDeResultados

resultado:
	$(CC) $(CFLAGS) -c common/Resultado.cpp
	
programa:
	$(CC) $(CFLAGS) -c common/Programa.cpp	
	
iPlanillaA: logger lockfile
	$(CC) $(CFLAGS) -c common/iPlanillaTesterA.cpp	
	
iPlanillaB: logger lockfile
	$(CC) $(CFLAGS) -c common/iPlanillaTesterB.cpp	

testerA: programa lockfile logger despachadorTester atendedorTest iPlanillaA
	$(CC) $(CFLAGS) Programa.o LockFile.o logger/Logger.o AtendedorTesters.o iPlanillaTesterA.o TesterA.cpp -o TesterA	
	
testerB: resultado lockfile logger despachadorTester atendedorTest iPlanillaB
	$(CC) $(CFLAGS) Resultado.o LockFile.o logger/Logger.o DespachadorTesters.o AtendedorTesters.o iPlanillaTesterB.o TesterB.cpp -o TesterB

planilla: semaphore
	$(CC) $(CFLAGS) -c common/Planilla.cpp	

planillaA: planilla
	$(CC) $(CFLAGS) Semaphore.o Planilla.o PlanillaTesterA.cpp -o PlanillaTesterA

planillaB: planilla
	$(CC) $(CFLAGS) Semaphore.o Planilla.o PlanillaTesterB.cpp -o PlanillaTesterB

iniciador: semaphore logger lockfile
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o iniciador.cpp -o iniciador

terminador: semaphore lockfile logger
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o terminador.cpp -o terminador
cleanO:
	rm -f *.o

clean: cleanO
	rm Dispositivo
	rm Tecnico
	rm ArriboDeResultados
	rm TesterA
	rm TesterB
	rm PlanillaTesterA
	rm PlanillaTesterB
	rm iniciador
	rm terminador
