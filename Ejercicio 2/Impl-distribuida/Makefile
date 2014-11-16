CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG

all: dispositivo tecnico arriboResul testerA testerB planillaA planillaB lectorNuevosReqD lectorNuevosReqT lectorProgD lectorProgT lectorOrdD lectorOrdT lectorResulD lectorResulT iniciador terminador cleanO

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

arriboResul: semaphore lockfile logger
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o ArriboDeResultados.cpp -o ArriboDeResultados

resultado:
	$(CC) $(CFLAGS) -c common/Resultado.cpp
	
programa:
	$(CC) $(CFLAGS) -c common/Programa.cpp	
	
iPlanillaA:
	$(CC) $(CFLAGS) -c common/iPlanillaTesterA.cpp	
	
iPlanillaB:
	$(CC) $(CFLAGS) -c common/iPlanillaTesterB.cpp	

testerA: programa lockfile logger despachadorTester atendedorTest iPlanillaA
	$(CC) $(CFLAGS) Programa.o LockFile.o logger/Logger.o AtendedorTesters.o iPlanillaTesterA.o TesterA.cpp -o TesterA	
	
testerB: resultado lockfile logger despachadorTester atendedorTest iPlanillaB
	$(CC) $(CFLAGS) Resultado.o LockFile.o logger/Logger.o DespachadorTesters.o AtendedorTesters.o iPlanillaTesterB.o TesterB.cpp -o TesterB

planilla: semaphore
	$(CC) $(CFLAGS) -c common/Planilla.cpp	

planillaA: planilla lockfile logger
	$(CC) $(CFLAGS) Semaphore.o Planilla.o LockFile.o logger/Logger.o PlanillaTesterA.cpp -o PlanillaTesterA

planillaB: planilla lockfile logger
	$(CC) $(CFLAGS) Semaphore.o Planilla.o LockFile.o logger/Logger.o PlanillaTesterB.cpp -o PlanillaTesterB

iniciador: semaphore logger lockfile
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o iniciador.cpp -o iniciador

terminador: semaphore lockfile logger
	$(CC) $(CFLAGS) Semaphore.o LockFile.o logger/Logger.o terminador.cpp -o terminador
	
lectorNuevosReqD: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorNuevosRequerimientos-Disp.cpp -o ProgramasLectores/LectorNuevosRequerimientos-Disp

lectorNuevosReqT: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorNuevosRequerimientos-Tester.cpp -o ProgramasLectores/LectorNuevosRequerimientos-Tester
	
lectorProgD: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorProgramas-Disp.cpp -o ProgramasLectores/LectorProgramas-Disp

lectorProgT: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorProgramas-Tester.cpp -o ProgramasLectores/LectorProgramas-Tester

lectorOrdD: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorOrdenes-Disp.cpp -o ProgramasLectores/LectorOrdenes-Disp

lectorOrdT: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorOrdenes-Tester.cpp -o ProgramasLectores/LectorOrdenes-Tester

lectorResulD: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorResultados-Disp.cpp -o ProgramasLectores/LectorResultados-Disp

lectorResulT: lockfile logger
	$(CC) $(CFLAGS) LockFile.o logger/Logger.o ProgramasLectores/LectorResultados-Tester.cpp -o ProgramasLectores/LectorResultados-Tester


cleanO:
	rm -f *.o

clean: cleanO
	rm -f log.txt
	rm -f Dispositivo
	rm -f Tecnico
	rm -f ArriboDeResultados
	rm -f TesterA
	rm -f TesterB
	rm -f PlanillaTesterA
	rm -f PlanillaTesterB
	rm -f ProgramasLectores/LectorNuevosRequerimientos-Disp
	rm -f ProgramasLectores/LectorNuevosRequerimientos-Tester
	rm -f ProgramasLectores/LectorProgramas-Disp
	rm -f ProgramasLectores/LectorProgramas-Tester
	rm -f ProgramasLectores/LectorOrdenes-Disp
	rm -f ProgramasLectores/LectorOrdenes-Tester
	rm -f ProgramasLectores/LectorResultados-Disp
	rm -f ProgramasLectores/LectorResultados-Tester
	rm -f iniciador
	rm -f terminador

reprocesar:
	./terminador
	rm -f log.txt
	./iniciador
