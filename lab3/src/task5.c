#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    //printf("after");
    pid_t child_pid = fork();
    if(child_pid>=0)
    {
        if(child_pid==0)
        {
            execv("sequential_min_max",argv);
        }
    }
    wait(0);
    return 0;    
}

//gcc -o task5 -c task5.c
//gcc task5 -o task5.out
//./task5.out 1 4