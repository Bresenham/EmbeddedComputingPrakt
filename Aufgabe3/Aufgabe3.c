#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <sched.h>

double time_in_ms_for_one_million = 0;

void useless(unsigned long runs) {
	unsigned long sum = 0;
	unsigned long i = 0;
	for(i = 0; i < runs; i++) {
		sum += i;
	}
}

void waste_msecs(unsigned int msecs) {
	unsigned long runs = ((double)(msecs) / time_in_ms_for_one_million) * 1000 * 1000;
	useless(runs);
}

double measure_useless_for_one_million() {
	/* Calculate time for 1.000.000 runs */
	struct timespec start_time, end_time;
	int clk_return = clock_gettime(CLOCK_REALTIME, &start_time);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clk_return != 0) {
		printf("Clock_gettime returned %d\r\n", clk_return);
		exit(-1);
	}

	useless(1000 * 1000);

	clk_return = clock_gettime(CLOCK_REALTIME, &end_time);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clk_return != 0) {
		printf("Clock_gettime returned %d\r\n", clk_return);
		exit(-1);
	}

	unsigned int difference_in_seconds = end_time.tv_sec - start_time.tv_sec;
	unsigned long difference_in_ns = end_time.tv_nsec - start_time.tv_nsec;

	return (difference_in_seconds * 1000 * 1000 * 1000 + difference_in_ns) / (1000.0 * 1000.0);
}

void* thread_function(void *arg) {
	const pthread_t self = pthread_self();
	int i = 0;
	double spent = 0.0;
	printf("--- THREAD TALKING ---\r\n");
	/* Measure accuracy of measure_useless_for_one_million */
	struct sched_param thread_sched;
	const int ret = pthread_getschedparam(self, NULL, &thread_sched);
	if(ret != 0) {
		printf("pthread_getschedparam returned %d\r\n", ret);
		exit(-1);
	}
	printf("My priority is %d\r\n", thread_sched.sched_priority);

	for(i = 0; i <= 99; i++) {
		const double current = measure_useless_for_one_million();
		printf("CURRENT MEASUREMENT: %fms\r\n", current);
		spent += current;
	}

	time_in_ms_for_one_million = spent / i;
	printf("MEAN VALUE: %fms\r\n", time_in_ms_for_one_million);
	return NULL;
}

int main(int argc, char *argv[]) {
	int i = 0;
	double spent = 0.0;

	/* Get main-thread priority */
	struct sched_param thread_sched;
	int ret_sched = pthread_getschedparam(pthread_self(), NULL, &thread_sched);
	if(ret_sched != 0) {
		printf("pthread_getschedparam returned %d\r\n", ret_sched);
		exit(-1);
	}
	printf("Main thread priority is %d\r\n", thread_sched.sched_priority);

	/* Measure accuracy of measure_useless_for_one_million */
	for(i = 0; i <= 99; i++) {
		const double current = measure_useless_for_one_million();
		printf("CURRENT MEASUREMENT: %fms\r\n", current);
		spent += current;
	}
	time_in_ms_for_one_million = spent / i;
	printf("MEAN VALUE: %fms\r\n", time_in_ms_for_one_million);


	// Teil b) mit Thread-Prioritaet

	pthread_attr_t thread_attr;
	const int attr_init_result = pthread_attr_init(&thread_attr);
	if (attr_init_result != 0) {
		printf("attr_init_result: %s\n", strerror(attr_init_result));
		exit(-1);
	}

	// Vererbung der Scheduling-Parameter ausschalten
	int ret = pthread_attr_setinheritsched(&thread_attr,PTHREAD_EXPLICIT_SCHED);
	if(ret != 0) {
		printf("pthread_attr_setinheritsched returned %d\r\n", ret);
		exit(-1);
	}

	// Aktuelle Prioritaet des Threads erfragen
	ret = pthread_attr_getschedparam(&thread_attr, &thread_sched);
	if(ret != 0) {
		printf("pthread_attr_getschedparam returned %d\r\n", ret);
		exit(-1);
	}

	// Nur die Prioritaet aendern und zurueckschreiben
	thread_sched.sched_priority = 253;
	ret = pthread_attr_setschedparam(&thread_attr, &thread_sched);
	if(ret != 0) {
		printf("pthread_attr_setschedparam returned %d\r\n", ret);
		exit(-1);
	}

	const int attr_setstate_result = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
	if (attr_setstate_result != 0) {
		printf("pthread_attr_setdetachstate: %s\n", strerror(attr_setstate_result));
		exit(-1);
	}

	pthread_t thread;
	const int create_thread_result = pthread_create(&thread, &thread_attr, &thread_function, NULL);
	if (create_thread_result != 0) {
		printf("create_thread_result: %s\n", strerror(create_thread_result));
		exit(-1);
	}

	const int pthread_join_result = pthread_join(thread, NULL);
	if (pthread_join_result != 0) {
		printf("pthread_join_result: %s\n", strerror(pthread_join_result));
		exit(-1);
	}

	return EXIT_SUCCESS;
}
