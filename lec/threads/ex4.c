/******************************************
 *
 *            Example Four
 *
 *  A solution to the producer consumer
 *  problem using condition variables
 ****************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 5
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
int count = 0;

void *producer(void* tid) {
	int i;
	for(i=0; i<20; i++) {
		pthread_mutex_lock(&mutex);
		if(count == SIZE) {
			printf("Producer: buffer is full\n");
			pthread_cond_wait(&empty, &mutex);
		}
		count = count+1;
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

void *consumer(void *tid) {
	int i;
	for(i=0; i<20; i++) {
		pthread_mutex_lock(&mutex);
		if(count == 0) {
			printf("Consumer: buffer is empty\n");
			pthread_cond_wait(&full, &mutex);
		}
		count = count-1;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t pro;
	pthread_t con;
	int rc;
	rc = pthread_create(&pro, NULL, producer, NULL);
	if(rc) {
		printf("Error: can't create producer\n");
		exit(-1);
	}
	rc = pthread_create(&con, NULL, consumer, NULL);
	if(rc) {
		printf("Error: can't create consumer\n");
		exit(-1);
	}
	pthread_exit(NULL);
}
