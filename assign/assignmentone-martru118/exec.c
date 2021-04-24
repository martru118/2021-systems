/**************************************************
 *
 *                   exec.c
 *
 *  The exec function that executes a command
 *  pipeline.
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mysh.h"

int runPipe(int, int, struct Command*);
void cmdArgs(struct Command*, char**);
void redirect(struct Command*);

/*
 *  Execute a pipeline.
 */
int exec(struct Pipeline *line) {
    struct Command* c = line->commands;

    //initialize file descriptors
    int in = STDIN_FILENO;
    int fd[2] = {STDIN_FILENO, STDOUT_FILENO};
    int piped = 0;      //false

    while (c!=NULL && c->next!=NULL) {
        //pipe commands
        pipe(fd);
        runPipe(in, fd[1], c);

        //change file descriptors
        close(fd[1]);
        in = fd[0];

        piped = 1;      //true
        c = c->next;
    }

    //execute last stage of pipeline
    runPipe(in, fd[1], c);
    if (piped == 1) close(fd[0]);
    return(0);
}

/*
 *  Runs a command in pipeline based on file descriptor.
 */
int runPipe(int in, int out, struct Command* c) {
    char* args[64] = {0};
    cmdArgs(c, args);

    //start child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("ERROR: Could not fork process... \n");
        exit(1);
    } else if (pid == 0) {
        //duplicate based on file descriptor
        if (in != STDIN_FILENO) {
            dup2(in, STDIN_FILENO);
            close(in);
        }

        if (out != STDOUT_FILENO) {
            dup2(out, STDOUT_FILENO);
            close(out);
        }

        //handle special keywords
        redirect(c);
        if (strcmp(args[0], "prompt") == 0) exit(0);
        if (strcmp(args[0], "exit") == 0) kill(pid, SIGTERM);

        //error handling
        if (execvp(args[0], args) < 0) {
            printf("ERROR: Could not execute command... \n");
            exit(1);
        }
    } else {
        //change prompt string
        if (strcmp(args[0], "prompt")==0 && args[1]!=NULL) Prompt = args[1];
        
        wait(NULL);
    }

    return pid;
}

/*
 *  Builds a command with arguments, if any.
 */
void cmdArgs(struct Command* c, char** buffer) {
    struct Arg* current = c->args;
    buffer[0] = c->name;

    int i = 1;

    //add args to command, if any
    while (current != NULL) {
        buffer[i] = current->name;

        i++;
        current = current->next;
    }
}

/*
 *  Perform I/O redirection.
 */
void redirect(struct Command* c) {
    char* input = c->input;
    char* output = c->output;

    //redirect input
    if (input != NULL) {
        int fin = open(input, O_RDONLY | O_CREAT, 0777);

        dup2(fin, STDIN_FILENO);
        close(fin);
    }

    //redirect output
    if (output != NULL) {
        int fout = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        dup2(fout, STDOUT_FILENO);
        close(fout);
    }
}