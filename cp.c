#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char* source = argv[1];
	char* destination = argv[2];
	int fd = open(source, O_RDWR|O_CREAT, S_IRWXU);
	if(fd < 0){
	perror("read error");
		return errno;
	}
	struct stat file_status;
	int status = stat(source, &file_status);
	if (status){
		perror("source error");
		return errno;
	}
	int size = file_status.st_size;

	void *buff = malloc(size);
	int re = 0;
	while(re < size){
		 re = read(fd, buff, size);
		if ( re < 0){
			perror("read error");
			return errno;
		}
	}


	int fd1 = open(destination, O_RDWR|O_CREAT, S_IRWXU);
	if(fd1 < 0){
	perror("eroare");
		return errno;
	}
		
	int wr = 0;
	while(wr < size){
		wr = write(fd1, buff, size);
			if (wr < 0){
				perror("write error");
				return errno;
		}
	}
		
	close(fd);
	close(fd1);
	free(buff);
	return 0;
}
