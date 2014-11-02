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

ARRIBO_SOURCES=src/ArriboDeResultados.cpp
ARRIBO_OBJ=$(ARRIBO_SOURCES:.cpp=.o)
ARRIBO_BIN=$(BINF)/arribo

PLANILLAA_SOURCES=src/PlanillaTesterA.cpp
PLANILLAA_OBJ=$(PLANILLAA_SOURCES:.cpp=.o)
PLANILLAA_BIN=$(BINF)/planillaa

PLANILLAB_SOURCES=src/PlanillaTesterB.cpp
PLANILLAB_OBJ=$(PLANILLAB_SOURCES:.cpp=.o)
PLANILLAB_BIN=$(BINF)/planillab

TESTERA_SOURCES=src/TesterA.cpp
TESTERA_OBJ=$(TESTERA_SOURCES:.cpp=.o)
TESTERA_BIN=$(BINF)/testerA

TESTERB_SOURCES=src/TesterB.cpp
TESTERB_OBJ=$(TESTERB_SOURCES:.cpp=.o)
TESTERB_BIN=$(BINF)/testerB

TESTERESP_SOURCES=src/TesterEspecial.cpp
TESTERESP_OBJ=$(TESTERESP_SOURCES:.cpp=.o)
TESTERESP_BIN=$(BINF)/testerEspecial

TECNICO_SOURCES=src/Tecnico.cpp
TECNICO_OBJ=$(TECNICO_SOURCES:.cpp=.o)
TECNICO_BIN=$(BINF)/tecnico

FIN_SOURCES=src/terminador.cpp
FIN_OBJ=$(FIN_SOURCES:.cpp=.o)
FIN_BIN=$(BINF)/terminador

all: testerA testerB testerEspecial tecnico dispositivo iniciador terminador arribo planillaa planillab

iniciador: $(INIT_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(INIT_OBJ) -o $(INIT_BIN) -I$(HEADER_DIR)

dispositivo: $(DISP_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_OBJ) -o $(DISP_BIN) -I$(HEADER_DIR)
	
arribo: $(ARRIBO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(ARRIBO_OBJ) -o $(ARRIBO_BIN) -I$(HEADER_DIR)
	
planillaa: $(PLANA_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(PLANA_OBJ) -o $(PLANA_BIN) -I$(HEADER_DIR)

planillab: $(PLANB_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(PLANB_OBJ) -o $(PLANB_BIN) -I$(HEADER_DIR)

testerA: $(TESTERA_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERA_OBJ) $(SHARED_OBJ) -o $(TESTERA_BIN) -I$(HEADER_DIR)
	
testerB: $(TESTERB_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERB_OBJ) $(SHARED_OBJ) -o $(TESTERB_BIN) -I$(HEADER_DIR)
	
testerEspecial: $(TESTERESP_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERESP_OBJ) $(SHARED_OBJ) -o $(TESTERESP_BIN) -I$(HEADER_DIR)

tecnico: $(TECNICO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(TECNICO_OBJ) -o $(TECNICO_BIN) -I$(HEADER_DIR)

terminador: $(FIN_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(FIN_OBJ) -o $(FIN_BIN) -I$(HEADER_DIR)

.cpp.o:
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

clean:
	$(RM) $(INIT_OBJ) $(INIT_BIN) $(DISP_OBJ) $(DISP_BIN) $(ARRIBO_OBJ) $(ARRIBO_BIN) $(PLANA_OBJ) $(PLANA_BIN) $(PLANB_OBJ) $(PLANB_BIN) $(TESTERA_OBJ) $(TESTERA_BIN) $(TESTERB_OBJ) $(TESTERB_BIN) $(TESTERESP_OBJ) $(TESTERESP_BIN) $(TECNICO_OBJ) $(TECNICO_BIN) $(FIN_BIN) $(FIN_OBJ) $(SHARED_OBJ)
