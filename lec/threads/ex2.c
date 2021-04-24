/**************************************************
 *
 *              Example Two
 *
 *  This example creates a collection of threads
 *  where each thread returns its id and the main
 *  procedure waits for each thread to exit.
 ************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5

/*
 *  The thread procedure, called for each thread
*/
void *printHello(void *threadid) {
	long tid;
	tid = (long) threadid;
	printf("Hello World from thread #%ld\n", tid);
	pthread_exit(threadid);
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	void *status;
	for(t=0; t<NUM_THREADS; t++) {
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, printHello, (void*) t);
		if(rc) {
			printf("Error in pthread_create: %d\n",rc);
			exit(-1);
		}
	}
	for(t=0; t<NUM_THREADS; t++) {
		rc = pthread_join(threads[t], &status);
		if(rc) {
			printf("Error in pthread_join: %d\n",rc);
			exit(-1);
		}
		printf("Main: thread %ld completed with status %ld\n",
			t, (long) status);
	}
	pthread_exit(NULL);
}
