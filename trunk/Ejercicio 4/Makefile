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

DISP_SOURCES=src/dispositivo.cpp
DISP_OBJ=$(DISP_SOURCES:.cpp=.o)
DISP_BIN=$(BINF)/dispositivo

TESTER_ESP_SOURCES=src/tester_esp.cpp
TESTER_ESP_OBJ=$(TESTER_ESP_SOURCES:.cpp=.o)
TESTER_ESP_BIN=$(BINF)/tester_esp

TECNICO_SOURCES=src/tecnico.cpp
TECNICO_OBJ=$(TECNICO_SOURCES:.cpp=.o)
TECNICO_BIN=$(BINF)/tecnico

FIN_SOURCES=src/finalizador.cpp
FIN_OBJ=$(FIN_SOURCES:.cpp=.o)
FIN_BIN=$(BINF)/finalizador

all: iniciador dispositivo finalizador tester_esp tecnico

iniciador: $(INIT_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(INIT_OBJ) -o $(INIT_BIN) -I$(HEADER_DIR)

dispositivo: $(DISP_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(DISP_OBJ) -o $(DISP_BIN) -I$(HEADER_DIR)

tester_esp: $(TESTER_ESP_OBJ) $(SHARED_OBJ)
	$(CC) $(TESTER_ESP_OBJ) $(SHARED_OBJ) -o $(TESTER_ESP_BIN) -I$(HEADER_DIR)

tecnico: $(TECNICO_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(TECNICO_OBJ) -o $(TECNICO_BIN) -I$(HEADER_DIR)

finalizador: $(FIN_OBJ) $(SHARED_OBJ)
	$(CC) $(SHARED_OBJ) $(FIN_OBJ) -o $(FIN_BIN) -I$(HEADER_DIR)

.cpp.o:
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -c $< -o $@

clean:
	$(RM) $(INIT_OBJ) $(INIT_BIN) $(DISP_OBJ) $(DISP_BIN) $(TESTER_ESP_OBJ) $(TESTER_ESP_BIN) $(TECNICO_OBJ) $(TECNICO_BIN) $(FIN_BIN) $(FIN_OBJ) $(SHARED_OBJ)
