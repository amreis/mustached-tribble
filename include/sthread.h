/*
 * sthread.h: arquivo de inclusão com os protótipos das funções a serem
 *            implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO 1 - 20/08/2014
 */
#ifndef __sthread__
#define __sthread__

#include "sdata.h"
int screate (int prio, void (*start)(void*), void *arg);
int syield(void);
int swait(int tid);
int smutex_init(smutex_t *mtx);
int slock (smutex_t *);
int sunlock (smutex_t *);

#endif
