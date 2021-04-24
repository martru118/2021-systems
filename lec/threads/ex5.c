/******************************************
 *
 *            Example Four
 *
 *  A solution to the producer consumer
 *  problem usinge semaphores
 ****************************************/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 5
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;

void *producer(void* tid) {
	int i;
	for(i=0; i<20; i++) {
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		printf("produce item\n");
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
	pthread_exit(NULL);
}

void *consumer(void *tid) {
	int i;
	for(i=0; i<20; i++) {
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		printf("consumer item\n");
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t pro;
	pthread_t con;
	int rc;
	sem_init(&empty, 0, SIZE);
	sem_init(&full, 0, 0);
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
