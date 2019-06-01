#include <stdio.h>
#include <stdlib.h>

double fac = 1.0;

void sleep_ms(int msec) {
	struct timespec time_to_wait;
	clock_gettime(CLOCK_REALTIME, &time_to_wait);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	while(msec > 0) {
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

    msec--;
	}
}

void waste_msecs(unsigned int msecs) {
	unsigned long runs = (unsigned long)(1000000 * fac * msecs);

	unsigned long sum = 0;
	unsigned long i = 0;
	for(i = 0; i < runs; i++) {
		sum += i;
	}
}

void measure_useless_for_one_million(unsigned int msec) {
	struct timespec start_time, end_time;
	int clk_return = clock_gettime(CLOCK_REALTIME, &start_time);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	waste_msecs(msec);

	clk_return = clock_gettime(CLOCK_REALTIME, &end_time);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	unsigned int difference_in_seconds = end_time.tv_sec - start_time.tv_sec;
	unsigned long difference_in_ns = end_time.tv_nsec - start_time.tv_nsec;
	unsigned long time = (difference_in_seconds * 1000 * 1000 * 1000 + difference_in_ns) / (1000.0);
	if(fac == 1.0) {
		fac = (double)msec / (double)(time / 1000.0);
		printf("Setting fac to %f\r\n", fac);
	}
	printf("Elapsed time %luus\r\n", time);
}

/*
  Thread 1: Zykluszeit 4ms, Verarbeitungszeit 2ms
*/
void* thread_1(void *arg) {
  unsigned long counter = 0;
  while(true) {
    // Schlafe 2ms
    sleep_ms(2);

    // Alle 3 Takte wird von Task 1 eine Semaphore gesetzt
    if(counter % 3 == 0) {
      // Setze Semaphore
    }

    // Verarbeit 2ms
    waste_msecs(2);
    counter++;
  }
}

/*
  Thread 2: wartet auf Semaphore von Task 1
*/
void* thread_2(void *arg) {
  while(true) {

  }
}

void create_thread(void* (thread_func)(void*)) {
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
	const int create_thread_result = pthread_create(&thread, &thread_attr, &thread_func, NULL);
	if (create_thread_result != 0) {
		printf("create_thread_result: %s\n", strerror(create_thread_result));
		exit(-1);
	}

	const int pthread_join_result = pthread_join(thread, NULL);
	if (pthread_join_result != 0) {
		printf("pthread_join_result: %s\n", strerror(pthread_join_result));
		exit(-1);
	}
}

int main(int argc, char *argv[]) {
	int i = 0;
	fac = 1.0;
	/* Set factor accordingly */
	measure_useless_for_one_million(100);

  create_thread(&thread_1);
  create_thread(&thread_2);

	return EXIT_SUCCESS;
}
