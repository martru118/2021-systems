#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int fin = 0;
	int fout = 1;
	int n = 1;
	char buffer[512];
	int ret;

	if(argc != 3) {
		printf("Usage: lab1 infile outfile\n");
		exit(1);
	}

	//get the process started
	while(n > 0) {
		n = read(fin, buffer, 512);
		if(n < 0) {
			printf("Error on read: %s\n",strerror(errno));
			exit(1);
		}

		ret = write(fout, buffer, n);
		if(ret < 0) {
			printf("Erroc on write: %s\n",strerror(errno));
			exit(1);
		}
	}

	close(fin);
	close(fout);
	
	exit(0);
}