#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char **argv)
{
    pid_t pid1;
    pid_t pid2;
    unsigned i;
    unsigned niterations = 1000;
    pid1 = fork();
    if (pid1 > 0)
    {
        pid2 = fork();
    }
    if(pid1 == 0) 
    {
        for(i = 0; i < niterations; ++i)
        printf("A = %d, ", i);
    }
    else if (pid2 == 0)
    {
        for(i = 0; i < niterations; ++i)
        printf("B = %d, ", i);
    }
    else
    {
        for(i = 0; i < niterations; ++i)
        printf("C = %d, ", i);

        printf("pid1 = %d and pid2 = %d\n ", pid1, pid2);
    }

    printf("\n");
}