#include "../include/sdata.h"
#include <stdio.h>

int main()
{
    tcb_queue q1, q1_end, q2, q2_end;
    q1 = NULL, q2 = NULL, q1_end = NULL, q2_end = NULL;
    TCB* t1 = (TCB*) malloc(sizeof(TCB));
    TCB* t2 = (TCB*) malloc(sizeof(TCB));
    TCB* t3 = (TCB*) malloc(sizeof(TCB));
    TCB* t4 = (TCB*) malloc(sizeof(TCB));
    TCB* t5 = (TCB*) malloc(sizeof(TCB));
    
    t1->priority = 2;
    t1->estado = STATE_READY;
    
    t2->priority = 0;
    t2->estado = STATE_READY;
    
    t3->priority = 1;
    t3->estado = STATE_BLOCKED;
    
    t4->priority = 0;
    t4->estado = STATE_BLOCKED;
    
    t5->priority = 2;
    t5->estado = STATE_BLOCKED;
    
    
    insert_queue(&q1, &q1_end, t1);
    insert_queue(&q1, &q1_end, t2);
    
    insert_queue(&q2, &q2_end, t3);
    insert_queue(&q2, &q2_end, t4);
    insert_queue(&q2, &q2_end, t5);
    
    while (q1 != NULL)
    {
        TCB* out;
        remove_queue(&q1, &q1_end, &out);
        printf("Removed thread with priority: %d and state: %d\n", out->priority, out->estado);
    }
    printf("\n");
    while (q2 != NULL)
    {
        TCB* out;
        remove_queue(&q2, &q2_end, &out);
        printf("Removed thread with priority: %d and state: %d\n", out->priority, out->estado);
    }
    TCB *out = NULL;
    remove_queue(&q2, &q2_end, &out);
    if (out == NULL)
        printf("OUT is null.\n");
    return 0;
}
