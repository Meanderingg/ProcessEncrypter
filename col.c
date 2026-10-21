#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(int argc, char * argv[]){

int value = atoi(argv[1]);
pid_t pid = fork ();

if ( pid < 0)
    return errno ;
else if ( pid == 0)
{
    /* execute the process */
    printf("%d ", value);
    fflush(stdout);
    if (value == 1)
    {
        printf("Procesul %d s-a terminat", getpid());
        kill(getpid(), SIGKILL);
    }
    if( value % 2 == 0)
    {
        value = value / 2;
    }
    else
    {
        value = (value * 3) + 1;
    }
    char num[32];
    sprintf(num, "%d", value);
    char * argv [] = {"collatz", num, NULL}; /*no itoa fct*/
    execve ("/home/tsan/Facultate/An2/SO/lab/lab4/collatz", argv, NULL);
    perror (NULL);
}

else
{
        /* parent instructions */
    /*printf("Child %d executed \n", pid);*/
    /*printf("Parent: %d \n", getpid());*/
}

for(int i = 1; i< argc; i++)
{

    wait(NULL);
}

    return 0;
}
