/*
 * sdata.h: arquivo de inclusão onde os grupos devem colocar as definições
 *          de suas estruturas de dados
 *
 * VERSÃO 1 - 20/08/2014
 */
 
 /*
  * Exemplo de estrutura de dados da Thread Control Block
  */
  #ifndef __SDATA_H__
  #define __SDATA_H__
  
  #define STATE_READY 0
  #define STATE_RUNNING 1
  #define STATE_BLOCKED 2
  
  #define MUTEX_IN_USE 1
  #define MUTEX_FREE 0
  
  #define PRIORITY_HIGH 0
  #define PRIORITY_MEDIUM 1
  #define PRIORITY_LOW 2
  

  
  #include <ucontext.h>
  #include <stdlib.h>
  
 typedef struct tcb {
    int tid; 
	int estado;
    ucontext_t* context;
    int priority;
	// ....
    int tid_waiting;
	struct tcb *next;
 } TCB;
 
/*
 * Exemplo de estrutura de dados "mutex"
 * Os grupos devem alterar essa estrutura de acordo com sua necessidade
 */ 
typedef struct mutex {
	int flag;
    TCB *first;
	TCB *last;
} smutex_t;

typedef TCB* tcb_queue;

int insert_queue(tcb_queue*, tcb_queue*, TCB*);
int pop_queue(tcb_queue*, tcb_queue*, TCB**);
int remove_queue(tcb_queue*, tcb_queue*, int, TCB**);

#endif
