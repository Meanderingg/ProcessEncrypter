#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_WORDS 10000

struct permutation {
    int* perm;
    int arrsize;
    int word_count;
};

struct permutation* permute (int begin, int end, char * contents)
{
    int arrsize = end-begin + 1;

    struct permutation *p = (struct permutation*) malloc(sizeof(struct permutation));
    p->perm = (int*) malloc(arrsize * sizeof(int));
    p->arrsize = arrsize;//struct for the permutation, array lenght and number which is permuted

    for(int i = 0; i < arrsize ; i++)
    {
        p->perm[i] = i;//creating the permutations array
    }

    srand(getpid());

    for(int i = arrsize - 1; i >= 0; i--)
    {
        //Fisher - Yates
        int j = rand() % (i+1);
        int temp = p->perm[i];
        int tempw = contents[begin + i];

        p->perm[i] = p->perm[j]; 
        p->perm[j] = temp; 

        //change the indeces for the contents
        contents[begin + i] = contents[begin + j];
        contents[begin + j] = tempw;
    }
    
    //printf("%d\n", perm[0]);
    //fflush(stdout);
    return p;
}

int main(int argc, char *argv[])
{
    /*
    printf("starting main\n");
    fflush(stdout);
    //forking code starts from the fork() call
    */

	char* source = argv[1];
    char* destination = malloc((strlen(source) + 5) * (sizeof(char)));//era +4 si mergea
    sprintf(destination, "%s.code", source);

	int fd = open(source, O_RDWR);
    int fd_code = open(destination, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);

	if(fd < 0){
	perror("read error");
		return errno;
	}
    if(fd_code < 0){
        perror("file error");
        return errno;
    }
    //open the file to be encrypted

	struct stat file_status;
	int status = stat(source, &file_status);

	if (status){
		perror("source error");
		return errno;
	}
    //get the stats of the file

	int size = file_status.st_size; //get the size for reading

    void * mapped_file = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); 
    //printf("Contents of the memory-mapped region: %s\n",(char*)mapped_file);//the mapping works

    //splitting the char based on words
    char* contents = (char*)mapped_file;

    int word_count = 0;
    int word_beginning = 0;
    int i = 0;

    while (i+1 < size) 
    { 
        //check if the word ended
        if((contents[i] == '\n' || contents[i] == '\t' || contents[i] == ' ') && contents[i+1] != ' ' && contents[i+1] != '\n' && contents[i+1] != '\t')
        {
            word_beginning = i + 1;
        }

        if( i != 0 && contents[i] != ' ' && contents[i] != '\n' && contents[i] != '\t')
        {
        if(contents[i + 1] == '\n' || contents[i+1] == ' ' || contents[i+1] == '\t') 
        {
            //end of word
            word_count++;

            int pipefd[2];
            if(pipe(pipefd) < 0){
                perror("Pipe");
                return errno;
            }

            //forking for each word
            pid_t pid = fork();

            if ( pid < 0)
            {
                perror("Fork error");
                return errno ;
            }
            else if ( pid == 0)
            {
                //child instruct
                close(pipefd[0]);//child does not read

                struct permutation* p = permute(word_beginning, i, contents); //function for permuting

                int perm_size = p->arrsize;
                p->word_count = word_count - 1; //first word is counted as word_count = 0, the word_count was already incremented
                
                write(pipefd[1], &p->word_count, sizeof(int));
                write(pipefd[1], &p->arrsize, sizeof(int));
                write(pipefd[1], p->perm, p->arrsize * sizeof(int));//sending the data to the parent

                close(pipefd[1]);
                raise(SIGKILL);
                //send a signal here
            }
            //setting the beginning of the next word

            else
            {
                //parent
                close(pipefd[1]);

                int child_count;
                int arrsize;

                read(pipefd[0], &child_count, sizeof(int));
                read(pipefd[0], &arrsize, sizeof(int)); //reading the word count and size

                write(fd_code, &child_count, sizeof(int));
                write(fd_code, &arrsize, sizeof(int));//writing the word count and the size of the word

                int *perm = malloc(arrsize * sizeof(int));
                read(pipefd[0], perm, arrsize * sizeof(int));//reding the permutation array


                ssize_t written = 0;
                while(written < arrsize * sizeof(int))
                {
                    ssize_t n = write(fd_code, (char*)perm + written, arrsize * sizeof(int) - written); //char* for propper counting
                    if(n < 0){
                        perror("Write error");
                        return errno;
                    }
                    written += n;
                }

                free(perm);
                close(pipefd[0]); 
            }
        }
        }
        i++;
    }


    for (int i = 0; i < word_count; i++)
    {
        pid_t k = wait(NULL); //waiting for the children to end
    }
    
    int wr = write(fd_code, &word_count, sizeof(int));//appending the word count to the file
    if(wr < 0)
    {
        perror("Write error");
        return errno;
    }
   
    //printf("Contents of the memory-mapped region: %s\n",(char*)mapped_file);//the mapping works
    /*
    int words = 0;
    lseek(fd_code, 0, SEEK_SET);//reset the file offset
    while(words < word_count){
        int count;
        int arrsize;

        read(fd_code, &count, sizeof(int));
        read(fd_code, &arrsize, sizeof(int));
        int* perm = malloc(arrsize * sizeof(int));
        read(fd_code, perm, arrsize * sizeof(int));
        
        printf("%d:", count);
        printf("%d:", arrsize);
        for(int i = 0; i < arrsize; i++)
        {
            printf("%d", perm[i]);
        }
        printf("\n");
        fflush(stdout);

        words ++;
    }
    */
    
    munmap(mapped_file, size);
    close(fd);
    close(fd_code);
    return 0;
}
