#include "sdata.h"
#include "sthread.h"

tcb_queue ready_queue[3] = {NULL, NULL, NULL};
tcb_queue ready_queue_end[3] = {NULL, NULL, NULL};
tcb_queue blocked_queue = NULL;
tcb_queue blocked_queue_end = NULL;
tcb_queue running = NULL;

ucontext_t* KILLTHREAD_CONTEXT = NULL;
TCB *main_thread = NULL;
int NEXT_TID = 0;

void scheduler()
{
    TCB* out = NULL;
    TCB* cur = NULL;
    int prio;
    for (prio = PRIORITY_HIGH; prio <= PRIORITY_LOW; ++prio)
    {
        if (ready_queue[prio] != NULL)
        {
            if (pop_queue(&ready_queue[prio], &ready_queue_end[prio], &out) == 0)
            {
                cur = running;
                running = out;
                if (cur != NULL)
                {
                    swapcontext(cur->context, out->context);
                    return;
                }
                else { setcontext(out->context); return; }
            }
            else return;
        }
    }
}

void killthread()
{

    if (running->tid_waiting != -1)
    {
        TCB *first;
        if (remove_queue(&blocked_queue, &blocked_queue_end,
                        running->tid_waiting, &first) == -1)
            return;
        first->estado = STATE_READY;
        insert_queue(&ready_queue[first->priority], &ready_queue_end[first->priority],
                        first);
        running->tid_waiting = -1;
    }
    free(running->context->uc_stack.ss_sp);
    free(running->context);
    free(running);
    running = NULL;
    scheduler();
}

int createMainThread()
{
    main_thread = (TCB*) malloc(sizeof(TCB));
    if (main_thread == NULL)
        return -1;
    main_thread->estado = STATE_RUNNING;
    main_thread->priority = PRIORITY_LOW;
    main_thread->tid = 0;
    main_thread->tid_waiting = -1;
    main_thread->next = NULL;
    running = main_thread;
    if ((main_thread->context = (ucontext_t*) malloc(sizeof(ucontext_t))) == NULL)
        return -1;
    getcontext(main_thread->context);
    return 0;
}

int init_thread(TCB* newThread, int prio, void (*start)(void*), void* arg)
{
    if (newThread == NULL)
        return -1;

    newThread->tid = ++NEXT_TID;
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
    newThread->next = NULL;
    newThread->tid_waiting = -1;
    return 0;
}

int validPriority(int prio)
{
    return (prio >= 0 && prio <= 2);
}

int screate(int prio, void (*start)(void*), void *arg)
{
    if (! validPriority(prio)) return -1;

    if (KILLTHREAD_CONTEXT == NULL)
    {
        KILLTHREAD_CONTEXT = (ucontext_t*) malloc(sizeof(ucontext_t));
        if (KILLTHREAD_CONTEXT == NULL)
            return -1;
        KILLTHREAD_CONTEXT->uc_link = NULL;
        KILLTHREAD_CONTEXT->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
        KILLTHREAD_CONTEXT->uc_stack.ss_size = SIGSTKSZ;

        getcontext(KILLTHREAD_CONTEXT);
        makecontext(KILLTHREAD_CONTEXT, (void (*)(void)) killthread, 0, NULL);
    }

    TCB *newThread = (TCB*) malloc(sizeof(TCB));
    if (init_thread(newThread, prio, start, arg) == -1)
        return -1;
    if (insert_queue(&ready_queue[prio], &ready_queue_end[prio], newThread) != 0)
        return -1;
    else
    {
        if (main_thread == NULL)
        {
            if (createMainThread() == 0)
                return newThread->tid;
            else return -1;
        }
        return newThread->tid;
    }
}


// Inicialização da variável mutex.
int smutex_init(smutex_t *mtx)
{
    mtx->flag = MUTEX_FREE;
    mtx->first = NULL;
    mtx->last = NULL;
    return 0;
}

int slock(smutex_t *mtx)
{
    if (main_thread == NULL)
    {
        if (createMainThread() != 0) return -1;
    }
    if (mtx->flag != MUTEX_FREE)
    {
        // Block current thread!
        do
        {
            running->estado = STATE_BLOCKED;
            if (insert_queue(&mtx->first, &mtx->last, running) != 0)
                return -1;
            scheduler();
        } while (mtx->flag == MUTEX_IN_USE);
        mtx->flag = MUTEX_IN_USE;
        return 0;
    }
    else
    {
        mtx->flag = MUTEX_IN_USE;
        return 0;
    }
    return 0;
}

int sunlock(smutex_t *mtx)
{
    if (main_thread == NULL)
    {
        if (createMainThread() != 0) return -1;
    }
    if (mtx->first != NULL)
    {
        TCB* first = NULL;
        pop_queue(&mtx->first, &mtx->last, &first);
        first->estado = STATE_READY;
        // Insert in the ready_queue
        insert_queue(&ready_queue[first->priority], &ready_queue_end[first->priority], first);
    }
    mtx->flag = 0;
    return 0;
}

int syield(void)
{
    if (running == NULL)
        return -1;
    TCB *out;
    out = running;
    out->estado = STATE_READY;
    if (insert_queue(&ready_queue[out->priority], &ready_queue_end[out->priority], out) != 0)
        return -1;
    scheduler();
    return 0;
}

int swait(int tid)
{
    // Refactor later!
    TCB *first;
    int i;
    for (i = PRIORITY_HIGH; i <= PRIORITY_LOW; ++i)
    {
        first = ready_queue[i];
        while (first != NULL)
        {
            if (first->tid == tid)
            {
                if (first->tid_waiting != -1)
                    return -1;
                first->tid_waiting = running->tid;
                TCB *me = running;
                me->estado = STATE_BLOCKED;
                if (insert_queue(&blocked_queue, &blocked_queue_end, me) != 0)
                    return -1;
                scheduler();
                return 0;
            }
            else first = first->next;
        }
    }

    first = blocked_queue;
    while (first != NULL)
    {
        if (first->tid == tid)
        {
            if (first->tid_waiting != -1)
                return -1;
            first->tid_waiting = running->tid;
            TCB *me = running;
            me->estado = STATE_BLOCKED;
            //running = NULL;
            if (insert_queue(&blocked_queue, &blocked_queue_end, me) != 0)
                return -1;
            scheduler();
            return 0;
        }
        else first = first->next;
    }
    // Didn't find thread to wait for, error.
    return -1;
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

int pop_queue(tcb_queue* queue_start, tcb_queue* queue_end, TCB** out)
{
    if (queue_start == NULL) return -1;

    if (*queue_start == NULL) return -1;

    else
    {
        *out = *queue_start;
        *queue_start = (*queue_start)->next;
        (*out)->next = NULL;
        if (*queue_start == NULL)
            *queue_end = NULL;
        return 0;
    }
}

int remove_queue(tcb_queue* queue_start, tcb_queue* queue_end, int tid, TCB** out)
{
    if (queue_start == NULL) return -1;

    if (*queue_start == NULL) return -1;

    if ((*queue_start)->tid == tid)
    {
        (*out) = *queue_start;
        *queue_start = (*queue_start)->next;
        if (*queue_start == NULL)
            *queue_end = NULL;
        return 0;
    }

    TCB *ant, *elem;
    ant = *queue_start;
    elem = ant->next;
    while (elem != NULL && elem->tid != tid)
    {
        ant = elem;
        elem = elem->next;
    }
    if (elem->tid == tid)
    {
        ant->next = elem->next;
        elem->next = NULL;
        *out = elem;
        return 0;
    }
    else return -1;
}
