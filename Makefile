#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: $(BIN_DIR)/sdata.o
	ar crs $(LIB_DIR)/libsthread.a $(BIN_DIR)/sdata.o
$(BIN_DIR)/sdata.o: $(SRC_DIR)/sdata.c
	$(CC) -I$(INC_DIR) -c -o $(BIN_DIR)/sdata.o $(SRC_DIR)/sdata.c -Wall
clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


