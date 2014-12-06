#TODO: El clean no funciona realmente: quedan los .o junto a los fuentes

CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG
LDFLAGS=
BINF=bin
HEADER_DIR=src

SHARED_SOURCES := $(shell find src/*/ -name '*.cpp')
SHARED_OBJ=$(SHARED_SOURCES:.cpp=.o)

INIT_SOURCES=src/iniciador.cpp
INIT_OBJ=$(INIT_SOURCES:.cpp=.o)
INIT_BIN=$(BINF)/iniciador

DISP_SOURCES=src/Dispositivo.cpp
DISP_OBJ=$(DISP_SOURCES:.cpp=.o)
DISP_BIN=$(BINF)/dispositivo

TESTER_SOURCES=src/Testeador.cpp
TESTER_OBJ=$(TESTER_SOURCES:.cpp=.o)
TESTER_BIN=$(BINF)/tester

TESTER2_SOURCES=src/Testeador2do.cpp
TESTER2_OBJ=$(TESTER2_SOURCES:.cpp=.o)
TESTER2_BIN=$(BINF)/tester_2do

TESTERRTA_SOURCES=src/TesteadorRespuesta.cpp
TESTERRTA_OBJ=$(TESTERRTA_SOURCES:.cpp=.o)
TESTERRTA_BIN=$(BINF)/tester_respuesta

PLANILLA1RO_SOURCES=src/PlanillaTester1ro.cpp
PLANILLA1RO_OBJ=$(PLANILLA1RO_SOURCES:.cpp=.o)
PLANILLA1RO_BIN=$(BINF)/planilla_tester_1ro

PLANILLA2DO_SOURCES=src/PlanillaTester2do.cpp
PLANILLA2DO_OBJ=$(PLANILLA2DO_SOURCES:.cpp=.o)
PLANILLA2DO_BIN=$(BINF)/planilla_tester_2do

PLANILLARTA_SOURCES=src/PlanillaTesterRespuesta.cpp
PLANILLARTA_OBJ=$(PLANILLARTA_SOURCES:.cpp=.o)
PLANILLARTA_BIN=$(BINF)/planilla_tester_rta

TECNICO_SOURCES=src/Tecnico.cpp
TECNICO_OBJ=$(TECNICO_SOURCES:.cpp=.o)
TECNICO_BIN=$(BINF)/tecnico

RESULTADOS_SOURCES=src/ArriboDeResultados.cpp
RESULTADOS_OBJ=$(RESULTADOS_SOURCES:.cpp=.o)
RESULTADOS_BIN=$(BINF)/arribo_de_resultados

RESULTADOSPARCIALES_SOURCES=src/ArriboDeResultadosParciales.cpp
RESULTADOSPARCIALES_OBJ=$(RESULTADOSPARCIALES_SOURCES:.cpp=.o)
RESULTADOSPARCIALES_BIN=$(BINF)/arribo_de_resultados_parciales

FIN_SOURCES=src/finalizador.cpp
FIN_OBJ=$(FIN_SOURCES:.cpp=.o)
FIN_BIN=$(BINF)/finalizador

all: iniciador dispositivo tester tester_2do tester_respuesta arribo_de_resultados arribo_de_resultados_parciales planilla_tester_1ro planilla_tester_2do planilla_tester_rta tecnico finalizador

iniciador: $(INIT_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(INIT_OBJ) -o $(INIT_BIN) -I$(HEADER_DIR)

dispositivo: $(DISP_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_OBJ) -o $(DISP_BIN) -I$(HEADER_DIR)

tester: $(TESTER_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTER_OBJ) $(SHARED_OBJ) -o $(TESTER_BIN) -I$(HEADER_DIR)
	

tester_2do: $(TESTER2_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTER2_OBJ) $(SHARED_OBJ) -o $(TESTER2_BIN) -I$(HEADER_DIR)
	

tester_respuesta: $(TESTERRTA_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERRTA_OBJ) $(SHARED_OBJ) -o $(TESTERRTA_BIN) -I$(HEADER_DIR)
	
arribo_de_resultados: $(RESULTADOS_OBJ) $(SHARED_OBJ)
	$(CC) $(RESULTADOS_OBJ) $(SHARED_OBJ) -o $(RESULTADOS_BIN) -I$(HEADER_DIR)

arribo_de_resultados_parciales: $(RESULTADOSPARCIALES_OBJ) $(SHARED_OBJ)
	$(CC) $(RESULTADOSPARCIALES_OBJ) $(SHARED_OBJ) -o $(RESULTADOSPARCIALES_BIN) -I$(HEADER_DIR)
	
planilla_tester_1ro: $(PLANILLA1RO_OBJ) $(SHARED_OBJ)
	$(CC) $(PLANILLA1RO_OBJ) $(SHARED_OBJ) -o $(PLANILLA1RO_BIN) -I$(HEADER_DIR)
	
planilla_tester_2do: $(PLANILLA2DO_OBJ) $(SHARED_OBJ)
	$(CC) $(PLANILLA2DO_OBJ) $(SHARED_OBJ) -o $(PLANILLA2DO_BIN) -I$(HEADER_DIR)
	
planilla_tester_rta: $(PLANILLARTA_OBJ) $(SHARED_OBJ)
	$(CC) $(PLANILLARTA_OBJ) $(SHARED_OBJ) -o $(PLANILLARTA_BIN) -I$(HEADER_DIR)

tecnico: $(TECNICO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(TECNICO_OBJ) -o $(TECNICO_BIN) -I$(HEADER_DIR)

finalizador: $(FIN_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(FIN_OBJ) -o $(FIN_BIN) -I$(HEADER_DIR)

.cpp.o:
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

clean:
	$(RM) $(INIT_OBJ) $(INIT_BIN) $(DISP_OBJ) $(DISP_BIN) $(TESTER_BIN) $(TESTER2_OBJ) $(TESTER2_BIN) $(TESTERRTA_BIN) $(RESULTADOS_BIN) $(RESULTADOSPARCIALES_BIN) $(PLANILLA1RO_BIN) $(PLANILLA2DO_BIN) $(PLANILLARTA_BIN) $(TECNICO_OBJ) $(TECNICO_BIN) $(FIN_BIN) $(FIN_OBJ) $(SHARED_OBJ) 
