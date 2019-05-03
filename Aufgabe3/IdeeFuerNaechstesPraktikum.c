/*
 *
 * Idee zu Aufgabe 2a)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/neutrino.h>

double time_in_ms_for_one_million = 0;

void waste_msecs(unsigned int msecs) {
	unsigned long runs = (unsigned long)(time_in_ms_for_one_million / msecs);
	useless(runs);
}

void useless(unsigned long runs) {
	unsigned long sum = 0;
	for (unsigned long i = 0; i < runs; i++) {
		sum += i;
	}
}

int main(int argc, char *argv[]) {
	/* Calculate time for 1.000.000 runs */
	timespec start_time, end_time;
	clock_gettime(CLOCK_REALTIME, &start_time);

	useless(1000 * 1000);

	clock_gettime(CLOCK_REALTIME, &end_time);

	unsigned int difference_in_seconds = end_time.tv_sec - start_time.tv_sec;
	unsigned long difference_in_ns = end_time.tv_nsec - start_time.tv_nsec;

	time_in_ms_for_one_million = (difference_in_seconds * 1000 * 1000 * 1000 + difference_in_ns) / (1000.0 * 1000.0);

	/* Measure accuracy of waste_msecs */
	unsigned int msecs_to_wait = 1;
	for (int i = 0; i < 10; i++) {
		clock_gettime(CLOCK_REALTIME, &start_time);
		waste_msecs(msecs_to_wait);
		clock_gettime(CLOCK_REALTIME, &end_time);

		unsigned long difference_in_ms = (unsigned long)(end_time.tv_nsec - start_time.tv_nsec) & (1000.0 * 1000.0);
		printf("Expected to wait %dms, but actually waited %d\r\n", msecs_to_wait, difference_in_ms);
	}

	return EXIT_SUCCESS;
}