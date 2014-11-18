CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors -g -ggdb -DDEBUG
LDFLAGS=
BINF=bin
HEADER_DIR=.

SHARED_SOURCES := $(shell find ./*/ -name '*.cpp')
SHARED_OBJ=$(SHARED_SOURCES:.cpp=.o)

INIT_SOURCES=iniciador.cpp
INIT_OBJ=$(INIT_SOURCES:.cpp=.o)
INIT_BIN=$(BINF)/iniciador

DISP_SOURCES=Dispositivo.cpp
DISP_OBJ=$(DISP_SOURCES:.cpp=.o)
DISP_BIN=$(BINF)/dispositivo

TESTERCOMUN_SOURCES=TesterComun.cpp
TESTERCOMUN_OBJ=$(TESTERCOMUN_SOURCES:.cpp=.o)
TESTERCOMUN_BIN=$(BINF)/testerComun

TESTERESP_SOURCES=TesterEspecial.cpp
TESTERESP_OBJ=$(TESTERESP_SOURCES:.cpp=.o)
TESTERESP_BIN=$(BINF)/testerEspecial

TECNICO_SOURCES=Tecnico.cpp
TECNICO_OBJ=$(TECNICO_SOURCES:.cpp=.o)
TECNICO_BIN=$(BINF)/tecnico

EQUIPOESP_SOURCES=EquipoEspecial.cpp
EQUIPOESP_OBJ=$(EQUIPOESP_SOURCES:.cpp=.o)
EQUIPOESP_BIN=$(BINF)/equipoEspecial

FIN_SOURCES=terminador.cpp
FIN_OBJ=$(FIN_SOURCES:.cpp=.o)
FIN_BIN=$(BINF)/terminador

all: testerComun testerEspecial tecnico dispositivo equipoEspecial iniciador terminador

iniciador: $(INIT_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(INIT_OBJ) -o $(INIT_BIN) -I$(HEADER_DIR)

dispositivo: $(DISP_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_OBJ) -o $(DISP_BIN) -I$(HEADER_DIR)
	
testerComun: $(TESTERCOMUN_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERCOMUN_OBJ) $(SHARED_OBJ) -o $(TESTERCOMUN_BIN) -I$(HEADER_DIR)
	
testerEspecial: $(TESTERESP_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERESP_OBJ) $(SHARED_OBJ) -o $(TESTERESP_BIN) -I$(HEADER_DIR)
	
equipoEspecial: $(EQUIPOESP_OBJ) $(SHARED_OBJ)
	$(CC) $(EQUIPOESP_OBJ) $(SHARED_OBJ) -o $(EQUIPOESP_BIN) -I$(HEADER_DIR)

tecnico: $(TECNICO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(TECNICO_OBJ) -o $(TECNICO_BIN) -I$(HEADER_DIR)

terminador: $(FIN_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(FIN_OBJ) -o $(FIN_BIN) -I$(HEADER_DIR)

.cpp.o:
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

clean:
	$(RM) $(INIT_OBJ) $(INIT_BIN) $(DISP_OBJ) $(DISP_BIN) $(TESTERCOMUN_OBJ) $(TESTERCOMUN_BIN) $(TESTERESP_OBJ) $(TESTERESP_BIN) $(EQUIPOESP_OBJ) $(EQUIPOESP_BIN) $(TECNICO_OBJ) $(TECNICO_BIN) $(FIN_BIN) $(FIN_OBJ) $(SHARED_OBJ)
