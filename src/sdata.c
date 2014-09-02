#include "../include/sthread.h"
#include "../include/sdata.h"

tcb_queue ready_queue[3] = {NULL, NULL, NULL};
tcb_queue ready_queue_end[3] = {NULL, NULL, NULL};
tcb_queue blocked_queue = NULL;
tcb_queue blocked_queue_end = NULL;
tcb_queue running = NULL;

ucontext_t* KILLTHREAD_CONTEXT = NULL;

unsigned int NEXT_TID = 0;

void scheduler()
{
    TCB* out = NULL;
    TCB* cur = NULL;
    if (ready_queue[0] != NULL)
    {
        if (remove_queue(&ready_queue[0], &ready_queue_end[0], &out) == 0)
        {
            cur = running;
            running = out;
            if (cur != NULL)
            {
                swapcontext(cur->context, out->context);
                return;
            }
            setcontext(out->context);
        }
        else return;
    }
    else if (ready_queue[1] != NULL)
    {
        if (remove_queue(&ready_queue[1], &ready_queue_end[1], &out) == 0)
        {
            cur = running;
            running = out;
            if (cur != NULL)
            {
                swapcontext(cur->context, out->context);
                return;
            }
            setcontext(out->context);
        }
        else return;
    }
    else if (ready_queue[2] != NULL)
    {
        if (remove_queue(&ready_queue[2], &ready_queue_end[2], &out) == 0)
        {
            cur = running;
            running = out;
            if (cur != NULL)
            {
                swapcontext(cur->context, out->context);
                return;
            }
            setcontext(out->context);
        }
        else return;
    }
    else
        return;
}

void killthread()
{
    free(running);
    running = NULL;
    scheduler();
}

int screate(int prio, void (*start)(void*), void *arg)
{
    if (prio < 0 || prio > 2) return -1;
    
    if (KILLTHREAD_CONTEXT == NULL)
    {
        KILLTHREAD_CONTEXT = (ucontext_t*) malloc(sizeof(ucontext_t));
        if (KILLTHREAD_CONTEXT == NULL)
            return -1;
        // Perhaps main's context?
        KILLTHREAD_CONTEXT->uc_link = NULL;
        KILLTHREAD_CONTEXT->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
        KILLTHREAD_CONTEXT->uc_stack.ss_size = SIGSTKSZ;
        
        getcontext(KILLTHREAD_CONTEXT);
        makecontext(KILLTHREAD_CONTEXT, (void (*)(void)) killthread, 0, NULL);
    }
    
    TCB *newThread = (TCB*) malloc(sizeof(TCB));
    if (newThread == NULL)
        return -1;
    
    newThread->tid = NEXT_TID++;
    newThread->estado = STATE_READY;
    newThread->context = (ucontext_t*) malloc(sizeof(ucontext_t));
    if (newThread->context == NULL)
        return -1;
    getcontext(newThread->context);
    // SHOULD CALL THE PROCESS KILLER
    newThread->context->uc_link = KILLTHREAD_CONTEXT;
    newThread->context->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    if (newThread->context->uc_stack.ss_sp == NULL)
        return -1;
    newThread->context->uc_stack.ss_size = SIGSTKSZ;
    
    makecontext(newThread->context, (void (*)(void))start, 1, arg);
    newThread->priority = prio;
    
    if (insert_queue(&ready_queue[prio], &ready_queue_end[prio], newThread) != 0)
        return -1;
    else
        return 0;
}


// Inicialização da variável mutex.
int smutex_init(smutex_t *mtx)
{
    if (mtx->first != NULL || mtx->last != NULL)
        return -1;
    mtx->flag = 0;
    mtx->first = NULL;
    mtx->last = NULL;
    return 0;
}

int slock(smutex_t *mtx)
{
    if (mtx->flag != 0)
    {
        // Block current thread!
        running->estado = STATE_BLOCKED;
        insert_queue(&mtx->first, &mtx->last, running);
        running = NULL;
    }
    else
    {
        mtx->flag = 1;
        return 0;
    }
    return 0;
}

int sunlock(smutex_t *mtx)
{
    if (mtx->first != NULL)
    {
        TCB* first = NULL;
        remove_queue(&mtx->first, &mtx->last, &first);
        first->estado = STATE_READY;
        // Insert in the ready_queue
        insert_queue(&ready_queue[first->priority], &ready_queue_end[first->priority], first);
    }
    mtx->flag = 0;
    return 0;
}

int insert_queue(tcb_queue* queue_start, tcb_queue* queue_end, TCB* item)
{
    if (queue_start == NULL) return -1;
    
    if (*queue_start == NULL)
    {
        item->next = NULL;
        *queue_start = item;
        *queue_end = item;
        return 0;
    }
    else
    {
        // Should have fallen on the first case (if).
        if (*queue_end == NULL) return -1;
        
        (*queue_end)->next = item;
        item->next = NULL;
        (*queue_end) = item;
        return 0;
    }
}

int remove_queue(tcb_queue* queue_start, tcb_queue* queue_end, TCB** out)
{
    if (queue_start == NULL) return -1;
    
    if (*queue_start == NULL) return -1;
    
    else
    {
        *out = *queue_start;
        *queue_start = (*queue_start)->next;
        if (*queue_start == NULL)
            *queue_end = NULL;
        return 0;
    }
}

