/*****************************************
 *
 *              Example Three
 *
 *  The readers and writers problems in
 *  pthreads.  The number of readers is
 *  a parameter.
 ***************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READERS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t db = PTHREAD_MUTEX_INITIALIZER;
int count = 0;

/*
 *  The writer thead procedure
 */
void *writer(void *threadid) {
	int i;
	for(i=0; i<10; i++) {
		sleep(1);	// prepare the data for writing
		pthread_mutex_lock(&db);
		printf("writer in database\n");
		printf("write out of database\n");
		pthread_mutex_unlock(&db);
	}
	pthread_exit(NULL);
}

/*
 *  The reader thread procedure
 */
void *reader(void *threadid) {
	long tid;
	int i;
	tid = (long) threadid;
	for(i=0; i<10; i++) {
		pthread_mutex_lock(&mutex);
		count = count+1;
		if(count == 1) pthread_mutex_lock(&db);
		pthread_mutex_unlock(&mutex);
		printf("reader %ld in database, total readers %d\n",tid,count);
		printf("reader %ld leaving database\n",tid);
		pthread_mutex_lock(&mutex);
		count = count -1;
		if(count == 0) pthread_mutex_unlock(&db);
		pthread_mutex_unlock(&mutex);
		sleep(1);	// process the data
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t theWriter;
	pthread_t readers[READERS];
	long t;
	int rc;
	rc = pthread_create(&theWriter, NULL, writer, (void*) t);
	if(rc) {
		printf("Error, writer failed to start: %d\n",rc);
		exit(-1);
	}
	for(t=0; t<READERS; t++) {
		rc = pthread_create(&readers[t], NULL, reader, (void*) t);
		if(rc) {
			printf("Error, reader failed to start: %d\n",rc);
			exit(-1);
		}
	}
	pthread_exit(NULL);
}
