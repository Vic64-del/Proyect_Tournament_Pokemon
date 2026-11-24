#variables 
CC = gcc
CFLAGS = -Wall -g -Ilib
TARGET = programa.exe
BUILD_DIR = build

#apartado de archivos fuentes de main y librerias
SRC_MAIN = src/main.c
SRC_STDTYPES = lib/stdtypes/stdtypes.c
SRC_STDPOKE = lib/stdpoke/stdpoke.c
SRC_STDTRAINER = lib/stdtrainer/stdtrainer.c  

#apartado de archivos objetos de cada libreria y main
OBJETOS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/stdtypes.o \
          $(BUILD_DIR)/stdpoke.o \
          $(BUILD_DIR)/stdtrainer.o          

#regla principal de compilacion
$(TARGET): $(OBJETOS)
	$(CC) $(CFLAGS) $^ -o $@


#primero se compila stdtypes, que no tiene dependencias!
$(BUILD_DIR)/stdtypes.o: $(SRC_STDTYPES) lib/stdtypes/stdtypes.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


#2da COMPILACION DE STDPOKE, QUE DEPENDE DE STDTYPES!
$(BUILD_DIR)/stdpoke.o: $(SRC_STDPOKE) lib/stdpoke/stdpoke.h lib/stdtypes/stdtypes.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Ilib/stdtypes -c $< -o $@


#3ERA COMPILACION: STDTRAINER QUE DEPENDE DE STDTYPES!
$(BUILD_DIR)/stdtrainer.o: $(SRC_STDTRAINER) lib/stdtrainer/stdtrainer.h lib/stdtypes/stdtypes.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Ilib/stdtypes -c $< -o $@


#Compilamos el main que depende de todas las librerias!
$(BUILD_DIR)/main.o: $(SRC_MAIN) lib/stdtypes/stdtypes.h lib/stdpoke/stdpoke.h lib/stdtrainer/stdtrainer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#comando para limpiar
clear:
	rm -rf $(BUILD_DIR) $(TARGET)

#comando para ejecutar
run: $(TARGET)
	./$(TARGET)

.PHONY: clear run