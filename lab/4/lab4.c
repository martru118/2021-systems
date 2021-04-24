#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

extern char **environ;

int main(int argc, char **argv) {
	int ret;
	int status;
	pid_t pid = fork();

	if(pid < 0) {
		printf("Fork error: %s\n",strerror(errno));
		exit(1);

		printf("Wait: %d\n", wait(&status));
	} else {
		//open input for reading
		int fin = open(argv[1], O_RDONLY | O_CREAT, 0777);
		if(fin < 0) {
			printf("Can't open input file: %s\n",strerror(errno));
			exit(1);
		}

		dup2(fin, 0);
		close(fin);
		
		//open output for writing
		int fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fout < 0) {
			printf("Can't open output file: %s\n",strerror(errno));
			exit(1);
		}

		dup2(fout, 1);
		close(fout);

		ret = execve("copy", argv, environ);
		if(ret < 0) {
			printf("Execve failed: %s\n", strerror(errno));
			exit(1);
		}
	}

	wait(NULL);
	exit(0);
}