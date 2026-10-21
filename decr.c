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

struct words{
    int begin;
    int end;
};

struct permutation {
    int* perm;
    int arrsize;
    int word_no;
};

int main(int argc, char *argv[])
{
    char* source = argv[1];
    char* perm = argv[2];
    char* destination = malloc((strlen(source) + 4) * (sizeof(char)));
    sprintf(destination, "%s.decr", source);//getting the encrypted file, permutations and output name

	int fd_source = open(source, O_RDONLY);
    int fd_perm = open(perm, O_RDONLY);
    int fd_dest = open(destination, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);//opening the files and creating the output file


	if(fd_source < 0){
	perror("read error");
		return errno;
	}
	if(fd_perm < 0){
	perror("read error");
		return errno;
	}
	if(fd_dest < 0){
	perror("read error");
		return errno;
	}

	struct stat source_status;
	struct stat perm_status;
	int s_status = stat(source, &source_status);
	int p_status = stat(perm, &perm_status);

	if (s_status){
		perror("source error");
		return errno;
	}
	if (p_status){
		perror("source error");
		return errno;
	}

	int source_size = source_status.st_size; 
	int perm_size = perm_status.st_size; //get the size for reading

    void * mapped_source = mmap(NULL, source_size, PROT_READ, MAP_PRIVATE, fd_source, 0); 
    char* contents = (char*)mapped_source; //getting the encripted words

    int word_count;
    off_t offset = - sizeof(int);
    off_t ls = lseek(fd_perm, offset, SEEK_END);
    if(ls < 0){
        perror("Lseek");
        return errno;
    }

    ssize_t rd = read(fd_perm, &word_count, sizeof(int));
    if(rd < 0){
        perror("Read error");
        return errno;
    }

    struct words* wd = (struct words*) malloc(word_count * sizeof(struct words));//an array for holding the word metadata

    int i = 0;
    wd[0].begin = 0;//the first word starts at 0
    int running_count = 0;
    while ( i + 1 < source_size && running_count < word_count) 
    { 
        //check if the word ended
        if((contents[i] == '\n' || contents[i] == '\t' || contents[i] == ' ') && contents[i+1] != ' ' && contents[i+1] != '\n' && contents[i+1] != '\t')
        {
            wd[running_count].begin = i + 1; //the current character is a space
        }

        if( i != 0 && contents[i] != ' ' && contents[i] != '\n' && contents[i] != '\t')
        {
        if(contents[i + 1] == '\n' || contents[i+1] == ' ' || contents[i+1] == '\t') 
        {
            //end of word
            wd[running_count].end = i;
            running_count++;
        }
        }
        i++;
    } //getting the words


    struct permutation* pm = (struct permutation*) malloc(word_count * sizeof(struct permutation));
    rd = 0; 

    ls = lseek(fd_perm, 0, SEEK_SET);//set the file pointer to the beginning
    if(ls < 0){
        perror("Lseek");
        return errno;
    }

    ssize_t n;
    while(rd + sizeof(int) < perm_size) //already read last int
    {
        int word_no;
        int arrsize;

        n = read(fd_perm, &word_no, sizeof(int));
        rd += n;
        if(n < 0)
            return errno;
        n = read(fd_perm, &arrsize, sizeof(int));
        rd += n;
        if(n < 0)
            return errno;

        int* permutation_array = malloc(arrsize * sizeof(int));//the permutation array
        pm[word_no].perm = permutation_array;
        pm[word_no].arrsize = arrsize;
        
        int p_read = 0;
        while(p_read < arrsize * sizeof(int)){
            ssize_t m = read(fd_perm, (char*)pm[word_no].perm + p_read, arrsize * sizeof(int) - p_read); //char* for propper counting
            if(m < 0)
                return errno;
            p_read += m;
        }

        rd += p_read;
    } //getting the permutations
    
    for (int i = 0; i < word_count; i++)
    {
        int size = pm[i].arrsize;
        int *inverse = malloc(size * sizeof(int));

        for (int j = 0; j < size; j++) {
            inverse[pm[i].perm[j]] = j;
        } // generating the inverse permutation

        for (int j = 0; j < size; j++) {
           // putchar(contents[ wd[i].begin + inverse[j]]);
            char c = contents[wd[i].begin + inverse[j]];
            if(write(fd_dest,&c, 1) < 0)
                return errno;
        }
        n = write(fd_dest,"\n",1);
        if(n < 0)
            return errno;
        free(inverse);
    }

    //for each pm free the perm
    munmap(mapped_source, source_size);
    free(wd);
    close(fd_source);
    close(fd_perm);
    close(fd_dest);
    return 0;
}
