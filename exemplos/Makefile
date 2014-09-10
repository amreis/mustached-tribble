#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
INC_DIR=../include

all: exemplo mandel teste_vetor teste_prio yield

exemplo: exemplo.c $(LIB_DIR)/libsthread.a
	$(CC) -o exemplo exemplo.c -L$(LIB_DIR) -lsthread -Wall -g

teste_vetor: teste_vetor.c $(LIB_DIR)/libsthread.a
	$(CC) -o teste_vetor teste_vetor.c -L$(LIB_DIR) -lsthread -Wall -g

teste_prio: teste_prio.c $(LIB_DIR)/libsthread.a
	$(CC) -o teste_prio teste_prio.c -L$(LIB_DIR) -lsthread -Wall -g

mandel: mandel.c $(LIB_DIR)/libsthread.a
	$(CC) -o mandel mandel.c -L$(LIB_DIR) -lsthread -lm -Wall -g
yield: teste_yield.c $(LIB_DIR)/libsthread.a
	$(CC) -o yield teste_yield.c -L$(LIB_DIR) -lsthread -lm -Wall -g
slock: test_slock.c $(LIB_DIR)/libsthread.a
	$(CC) -o slock test_slock.c -L$(LIB_DIR) -lsthread -Wall -g
	
clean:
	rm -rf exemplo mandel teste_vetor teste_prio *~
