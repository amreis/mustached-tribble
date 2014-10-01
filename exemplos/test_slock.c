#include <stdio.h>
#include "../include/sthread.h"

int global = 0;

smutex_t mutex;

void func4()
{
	printf("IMMA STEAL THAT MUTEX\n");
	slock(&mutex);
	printf("GOT IT\n");
	sunlock(&mutex);
	printf("LERIGOOOOO.\n");

}

void func3()
{
    printf("Lock3...\n");
    screate(0, func4, NULL);
    slock(&mutex);
    printf("Yield3...\n");
    syield();
    printf("Unlock3...\n");
    sunlock(&mutex);
}

void func()
{
	printf("Lock1...\n");
	slock(&mutex);
	printf("Yield1...\n");
	syield();
    screate(0, func3, NULL);
    syield();
	printf("Unlock1...\n");
    sunlock(&mutex);
}

void func2()
{
	printf("Lock2...\n");
	slock(&mutex);
    printf("Mutex flag: %d\n", mutex.flag);
	printf("Yield2...\n");
	syield();
	printf("Unlock2...\n");
    sunlock(&mutex);
    printf("Mutex flag after unlock: %d\n", mutex.flag);
}

int main()
{
	smutex_init(&mutex);
	int tid1, tid2;
	tid1 = screate(0, func, NULL);
	tid2 = screate(1, func2, NULL);
    swait(tid1);
	swait(tid2);
    return 0;
}
