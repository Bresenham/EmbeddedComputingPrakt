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

void runOneMillisecond() {
	while (true) {
		/* Get time before sleep to compare it afterwards */
		timespec time_before_sleep;
		clock_gettime(CLOCK_REALTIME, &time_before_sleep);
		printf("TIME BEFORE SLEEP: %ds %dns\r\n", time_before_sleep.tv_sec, time_before_sleep.tv_nsec);

		timespec time_to_wait, time_after_sleep;
		time_to_wait.tv_nsec = 1000 * 1000; /* Wait 1ms */
		clock_nanosleep(CLOCK_REALTIME, 0, &time_to_wait, NULL);

		/* Get time after sleep as soon as thread weaks up */
		clock_gettime(CLOCK_REALTIME, &time_after_sleep);
		printf("TIME AFTER 1ms SLEEP: %ds %dns\r\n", time_after_sleep.tv_sec, time_after_sleep.tv_nsec);
	}

	return NULL;
}

typedef struct _clockperiod {
	unsigned long nsec;
	long fract;
} _clockperiod;

void changeSystemTick(unsigned int microsecs) {
	/* Measure time resolution */
	_clockperiod get_period;
	ClockPeriod(CLOCK_REALTIME, NULL, &get_period, 0);
	printf("Clock period before changing is %dns\r\n", get_period.nsec);

	/* Set resolution to given value */
	_clockperiod set_period;
	set_period.nsec = microsecs * 1000;
	set_period.fract = 0;
	ClockPeriod(CLOCK_REALTIME, &set_period, NULL, 0);

	/* Measure time resolution again */
	ClockPeriod(CLOCK_REALTIME, NULL, &get_period, 0);
	printf("Clock period after changing is %dns\r\n", get_period.nsec);
}

/*
 TODO: Minimalen, gerade noch sinnvollen WErt für die Zykluszeit des Systemtakts ermitteln
*/
int main(int argc, char *argv[]) {
	pthread_attr_t thread_attr;
	const int attr_init_result = pthread_attr_init(&thread_attr);
	if (attr_init_result != 0) {
		printf("attr_init_result: %s\n", strerror(attr_init_result));
		exit(-1);
	}

	/* Brauchen wir hier DETACHED oder JOINABLE? Der Thread wird ja nie enden? */
	const int attr_setstate_result = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
	if (attr_setstate_result != 0) {
		printf("attr_setstate_result: %s\n", strerror(attr_setstate_result));
		exit(-1);
	}

	pthread_t thread;
	const int create_thread_result = pthread_create(&thread, &thread_attr, &runOneMillisecond, NULL);
	if (create_thread_result != 0) {
		printf("create_thread_result: %s\n", strerror(create_thread_result));
		exit(-1);
	}

	/* Wird nie zu Ende sein? */
	const int pthread_join_result = pthread_join(thread, NULL);
	if (pthread_join_result != 0) {
		printf("pthread_join_result: %s\n", strerror(pthread_join_result));
		exit(-1);
	}

	return EXIT_SUCCESS;
}