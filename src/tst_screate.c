#include "../include/sthread.h"
#include <stdio.h>

void start()
{
    int i;
    for (i = 0; i < 3; ++i)
        printf("Hello !\n");
}

int main()
{
    screate(0, (void(*)(void*))start, NULL);
    syield();
    return 0;
}
