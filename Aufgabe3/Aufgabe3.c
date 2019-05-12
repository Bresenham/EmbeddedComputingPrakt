#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/neutrino.h>
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
	unsigned long runs = (unsigned long)(time_in_ms_for_one_million / msecs);
	useless(runs);
}

double measure_useless_for_one_million() {
	/* Calculate time for 1.000.000 runs */
	struct timespec start_time, end_time;
	clock_gettime(CLOCK_REALTIME, &start_time);

	useless(1000 * 1000);

	clock_gettime(CLOCK_REALTIME, &end_time);

	unsigned int difference_in_seconds = end_time.tv_sec - start_time.tv_sec;
	unsigned long difference_in_ns = end_time.tv_nsec - start_time.tv_nsec;

	return (difference_in_seconds * 1000 * 1000 * 1000 + difference_in_ns) / (1000.0 * 1000.0);
}

void* thread_function(void *arg) {
	/* Zuvor uebergebenes pthread_attr_t casten */
	pthread_t self = pthread_self();

	struct sched_param thread_sched;
	pthread_getschedparam(&self, NULL, &thread_sched);
	printf("My priority is %d\r\n", thread_sched.sched_priority);

	double spent = measure_useless_for_one_million();
	printf("Time in ms for 1.000.000 useless-runs: %f\r\n", spent);

	return NULL;
}

int main(int argc, char *argv[]) {
	int i = 0;
	double spent = measure_useless_for_one_million();
	time_in_ms_for_one_million = spent;

	/* Measure accuracy of waste_msecs */
	struct timespec start_time, end_time;
	unsigned int msecs_to_wait = 1;
	for(i = 0; i < 10; i++) {
		clock_gettime(CLOCK_REALTIME, &start_time);
		waste_msecs(msecs_to_wait);
		clock_gettime(CLOCK_REALTIME, &end_time);

		unsigned long difference_in_ms = (unsigned long)(end_time.tv_nsec - start_time.tv_nsec) / (1000.0 * 1000.0);
		printf("Expected to wait %dms, but actually waited %d\r\n", msecs_to_wait, difference_in_ms);
	}


	/*
	 Teil b) mit Thread-Prioritaet
	 */
	pthread_attr_t thread_attr;
	const int attr_init_result = pthread_attr_init(&thread_attr);
	if (attr_init_result != 0) {
		printf("attr_init_result: %s\n", strerror(attr_init_result));
		exit(-1);
	}

	/* Vererbung der Scheduling-Parameter ausschalten */
	pthread_attr_setinheritsched(&thread_attr,PTHREAD_EXPLICIT_SCHED);

	/* Welche Policy soll hier verwendet werden? */
	int max_priority = sched_get_priority_max(SCHED_FIFO);

	/* Aktuelle Prioritaet des Threads erfragen */
	struct sched_param thread_sched;
	pthread_attr_getschedparam(&thread_attr, &thread_sched);

	/* Nur die Prioritaet aendern und zurueckschreiben */
	thread_sched.sched_priority = max_priority;
	pthread_attr_setschedparam(&thread_attr, &thread_sched);

	const int attr_setstate_result = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
	if (attr_setstate_result != 0) {
		printf("attr_setstate_result: %s\n", strerror(attr_setstate_result));
		exit(-1);
	}

	pthread_t thread;
	/* Keine Ahnung, ob man hier das pthread_attr_t uebergeben soll, aber in der Thread-Funktion habe ich es ja dann nicht mehr? */
	const int create_thread_result = pthread_create(&thread, &thread_attr, &thread_function, NULL);
	if (create_thread_result != 0) {
		printf("create_thread_result: %s\n", strerror(create_thread_result));
		exit(-1);
	}

	return EXIT_SUCCESS;
}
