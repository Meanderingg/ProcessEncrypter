#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char * argv[]){

if(argc < 2)
{
    printf("Please introduce a file to be encrypted\n");
    return 0;
}

if( argc > 3)
{
    printf("Use one file for encryption, or an ecrypted file and permutation scheme for decryption\n");
    return 0;
}

pid_t pid = fork ();

if ( pid < 0)
{
    perror("Fork error");
    return errno ;
}
else if ( pid == 0)
{
    //child instructions
    /* execute the process */

    if(argc == 2) //encryption
    {
    char * argve [] = {"encr", argv[1], NULL}; 
    execve ("/home/tsan/Facultate/An2/SO/lab/proiect/encr", argv, NULL);
    perror (NULL);
    }


    if(argc == 3) //decryption
    {
    char * argvd [] = {"decr", argv[1], argv[2], NULL}; 
    execve ("/home/tsan/Facultate/An2/SO/lab/proiect/decr", argvd, NULL);
    perror (NULL);
    }
}
else
{
        /* parent instructions */

    wait(NULL);
    //wating for the execution
}
    return 0;
}
