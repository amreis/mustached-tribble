#include "../include/sthread.h"
#include <stdio.h>

void print1(void *arg)
{
	int i;
	for (i = 0; i < 5; ++i)
	{
		printf("Thread %d\n", (int)arg);
		syield();
	}
}

int main()
{
	int tid[10];
	int i;
	for (i = 0; i < 10; ++i)
	{
		tid[i] = screate(1 - (i/5), print1, (void*)i);
	}
	printf("Main dando yield...\n");
	syield();
	printf("Main voltando do yield...\n");
	printf("Dando yield dnv pra ter ctz...\n");
	syield();
	printf("Que que deu?\n");
	return 0;
}
