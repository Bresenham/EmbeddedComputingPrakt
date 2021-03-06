#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void* p1Func(void *arg) {
	sleep(2);
	printf("Thread 1 calling\r\n");
	pthread_exit((void*)pthread_self());
	return NULL;
}

void* p2Func(void *arg) {
	sleep(3);
	printf("Thread 2 calling\r\n");
	pthread_exit((void*)pthread_self());
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_attr_t p1;
	const int attr_init_1_result = pthread_attr_init(&p1);
	if(attr_init_1_result != 0) {
		printf("attr_init_1_result: %s\n", strerror(attr_init_1_result));
		exit(-1);
	}

	const int attr_setstate_1_result = pthread_attr_setdetachstate(&p1, PTHREAD_CREATE_JOINABLE);
	if(attr_setstate_1_result != 0) {
		printf("attr_setstate_1_result: %s\n", strerror(attr_setstate_1_result));
		exit(-1);
	}

	pthread_attr_t p2;
	const int attr_init_2_result = pthread_attr_init(&p2);
	if(attr_init_1_result != 0) {
		printf("attr_init_2_result: %s\n", strerror(attr_init_2_result));
		exit(-1);
	}

	const int attr_setstate_2_result = pthread_attr_setdetachstate(&p2, PTHREAD_CREATE_JOINABLE);
	if(attr_setstate_2_result != 0) {
		printf("attr_setstate_2_result: %s\n", strerror(attr_setstate_2_result));
		exit(-1);
	}

	pthread_t p1Thread;
	pthread_t p2Thread;

	const int create_thread_1_result = pthread_create(&p1Thread, &p1, &p1Func, NULL);
	if(create_thread_1_result != 0) {
		printf("create_thread_1_result: %s\n", strerror(create_thread_1_result));
		exit(-1);
	}

	const int create_thread_2_result = pthread_create(&p2Thread, &p2, &p2Func, NULL);
	if(create_thread_2_result != 0) {
		printf("create_thread_2_result: %s\n", strerror(create_thread_2_result));
		exit(-1);
	}

	pthread_t p1IdAfter;
	pthread_t p2IdAfter;

	const int pthread_join_1_result = pthread_join(p1Thread, (void**)&p1IdAfter);
	if(pthread_join_1_result != 0) {
		printf("pthread_join_1_result: %s\n", strerror(pthread_join_1_result));
		exit(-1);
	}

	const int pthread_join_2_result = pthread_join(p2Thread, (void**)&p2IdAfter);
	if(pthread_join_2_result != 0) {
		printf("pthread_join_2_result: %s\n", strerror(pthread_join_2_result));
		exit(-1);
	}

	printf("P1-Before: %d\r\nP1-After: %d\r\n", p1Thread, p1IdAfter);
	printf("P2-Before: %d\r\nP2-After: %d\r\n", p2Thread, p2IdAfter);

	return EXIT_SUCCESS;
}
