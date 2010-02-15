/*
 ============================================================================
 Name        : pthread1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_barrier_t bar;

void *PrintHello(void *threadid)
{
	long tid;
	tid = (long)threadid;
	printf("Hello World Its me, thread #%ld!\n", tid);
	pthread_barrier_wait(&bar);
	pthread_exit(NULL);
}

int main(void) {
	pthread_t threads[5];
	pthread_barrier_init(&bar, NULL, 6);
	int rc;
	long t;
	for (t=0; t < 5; t++)
	{
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	pthread_barrier_wait(&bar);
	printf("All Threads Are Done");

	pthread_exit(NULL);
	return EXIT_SUCCESS;
}
