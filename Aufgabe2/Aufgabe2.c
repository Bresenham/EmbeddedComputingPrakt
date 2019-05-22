#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>

void* runOneMillisecond(void *arg) {
	struct timespec time_to_wait;
	clock_gettime(CLOCK_REALTIME, &time_to_wait);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	while(1) {
		time_to_wait.tv_nsec += 1000 * 1000;
		if(time_to_wait.tv_nsec >= 1000 * 1000 * 1000) {
			time_to_wait.tv_sec += 1;
			time_to_wait.tv_nsec -= 1000 * 1000 * 1000;
		}
		const int clk_nanosleep_return = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time_to_wait, NULL);
		if(clk_nanosleep_return != 0) {
			printf("clock_nanosleep returned %d\r\n", clk_nanosleep_return);
			exit(-1);
		}
	}
	return NULL;
}

void changeSystemTick(unsigned int microsecs) {
	/* Measure time resolution */
	struct _clockperiod get_period;
	ClockPeriod(CLOCK_REALTIME, NULL, &get_period, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	printf("Clock period before changing is %luus\r\n", (get_period.nsec / (1000)));

	/* Set resolution to given value */
	struct _clockperiod set_period;
	set_period.nsec = microsecs * 1000;
	set_period.fract = 0;
	ClockPeriod(CLOCK_REALTIME, &set_period, NULL, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	/* Measure time resolution again */
	ClockPeriod(CLOCK_REALTIME, NULL, &get_period, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	printf("Clock period after changing is %luus\r\n", (get_period.nsec / (1000)));
}

int main(int argc, char *argv[]) {
	changeSystemTick(1500);

	pthread_attr_t thread_attr;
	const int attr_init_result = pthread_attr_init(&thread_attr);
	if (attr_init_result != 0) {
		printf("attr_init_result: %s\n", strerror(attr_init_result));
		exit(-1);
	}

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

	const int pthread_join_result = pthread_join(thread, NULL);
	if (pthread_join_result != 0) {
		printf("pthread_join_result: %s\n", strerror(pthread_join_result));
		exit(-1);
	}

	return EXIT_SUCCESS;
}
