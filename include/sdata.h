/*
 * sdata.h: arquivo de inclusão onde os grupos devem colocar as definições
 *          de suas estruturas de dados
 *
 * VERSÃO 1 - 20/08/2014
 */
 
 /*
  * Exemplo de estrutura de dados da Thread Control Block
  */
 typedef struct tcb {
	int estado;
	// ....
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
} mmutex_t;
