/*******************************************
 *
 *                  main.c
 *
 *  The main procedure for the mysh program.
 ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mysh.h"

int main(int argc, char **argv) {
	struct Pipeline *pipe;
	struct stat status;
	
	/*
	 *  Create the initial prompt string
	 *  Determine whether we are connected to a terminal
	 */
	Prompt = (char*) malloc(2);
	strcpy(Prompt, "?");
	fstat(0,&status);

	/*
	 *  Loop parsing one line at a time and executing it
	 */
	while(1) {
		if(S_ISCHR(status.st_mode))
			printf("%s ",Prompt);
		pipe = parse(stdin);
		if(pipe == NULL)
			break;
		if(exec(pipe))
			break;
	}
	free(Prompt);
	return(0);
}
