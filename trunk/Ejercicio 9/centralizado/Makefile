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

TESTERCOMUN_SOURCES=src/TesterComun.cpp
TESTERCOMUN_OBJ=$(TESTERCOMUN_SOURCES:.cpp=.o)
TESTERCOMUN_BIN=$(BINF)/testerComun

TESTERESP_SOURCES=src/TesterEspecial.cpp
TESTERESP_OBJ=$(TESTERESP_SOURCES:.cpp=.o)
TESTERESP_BIN=$(BINF)/testerEspecial

TECNICO_SOURCES=src/Tecnico.cpp
TECNICO_OBJ=$(TECNICO_SOURCES:.cpp=.o)
TECNICO_BIN=$(BINF)/tecnico

EQUIPOESP_SOURCES=src/EquipoEspecial.cpp
EQUIPOESP_OBJ=$(EQUIPOESP_SOURCES:.cpp=.o)
EQUIPOESP_BIN=$(BINF)/equipoEspecial

TESTERCONFIG_SOURCES=src/TesterConfiguracion.cpp src/TestConfiguracion.cpp
TESTERCONFIG_OBJ=$(TESTERCONFIG_SOURCES:.cpp=.o)
TESTERCONFIG_BIN=$(BINF)/testerConfig

DISP_CONFIG_SOURCES=src/DispositivoConfig.cpp
DISP_CONFIG_OBJ=$(DISP_CONFIG_SOURCES:.cpp=.o)
DISP_CONFIG_BIN=$(BINF)/dispositivoConfig

FIN_SOURCES=src/finalizador.cpp
FIN_OBJ=$(FIN_SOURCES:.cpp=.o)
FIN_BIN=$(BINF)/finalizador

all: testerComun testerEspecial testerConfig tecnico dispositivo equipoEspecial iniciador finalizador dispositivoConfig

iniciador: $(INIT_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(INIT_OBJ) -o $(INIT_BIN) -I$(HEADER_DIR)

dispositivo: $(DISP_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_OBJ) -o $(DISP_BIN) -I$(HEADER_DIR)

dispositivoConfig: $(DISP_CONFIG_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_CONFIG_OBJ) -o $(DISP_CONFIG_BIN) -I$(HEADER_DIR)
	
testerComun: $(TESTERCOMUN_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERCOMUN_OBJ) $(SHARED_OBJ) -o $(TESTERCOMUN_BIN) -I$(HEADER_DIR)
	
testerEspecial: $(TESTERESP_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERESP_OBJ) $(SHARED_OBJ) -o $(TESTERESP_BIN) -I$(HEADER_DIR)

testerConfig: $(TESTERCONFIG_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTERCONFIG_OBJ) $(SHARED_OBJ) -o $(TESTERCONFIG_BIN) -I$(HEADER_DIR)

equipoEspecial: $(EQUIPOESP_OBJ) $(SHARED_OBJ)
	$(CC) $(EQUIPOESP_OBJ) $(SHARED_OBJ) -o $(EQUIPOESP_BIN) -I$(HEADER_DIR)

tecnico: $(TECNICO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(TECNICO_OBJ) -o $(TECNICO_BIN) -I$(HEADER_DIR)

finalizador: $(FIN_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(FIN_OBJ) -o $(FIN_BIN) -I$(HEADER_DIR)

.cpp.o:
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

clean:
	$(RM) $(INIT_OBJ) $(INIT_BIN) $(DISP_OBJ) $(DISP_BIN) $(TESTERCOMUN_OBJ) $(TESTERCOMUN_BIN) $(TESTERESP_OBJ) $(TESTERESP_BIN) $(EQUIPOESP_OBJ) $(EQUIPOESP_BIN) $(TECNICO_OBJ) $(TECNICO_BIN) $(FIN_BIN) $(FIN_OBJ) $(SHARED_OBJ)
