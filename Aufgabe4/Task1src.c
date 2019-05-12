/*
* Aufgabe 4
* Dieser Code wird von Task 1 ausgefuehrt.
* Ich weiss allerdings nicht, ob der neue Task auch eine 'main'-Funktion erwartet?
*/
#include <stdio.h>
#include <stdlib.h>

/* Diese Funktion verwenden um 2ms zu schlafen (Zykluszeit 4ms, Verarbeitungszeit 2ms => 2ms schlafen) */
void runTwoMillisecond() {
	while(1) {
	/* Get time before sleep to compare it afterwards */
	struct timespec time_before_sleep;
	// printf("TIME BEFORE SLEEP: %ds %dns\r\n", time_before_sleep.tv_sec, time_before_sleep.tv_nsec);
	int clk_return = clock_gettime(CLOCK_REALTIME, &time_before_sleep);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clk_return != 0) {
		printf("Clock_gettime returned %d\r\n", clk_return);
	}

	struct timespec time_to_wait, time_after_sleep;
	time_to_wait.tv_nsec = 2 * 1000 * 1000; /* Wait 2ms */
	const int clk_nanosleep_return = clock_nanosleep(CLOCK_REALTIME, 0, &time_to_wait, NULL);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clk_nanosleep_return != 0) {
		printf("Clock_nanosleep returned %d\r\n", clk_nanosleep_return);
	}

	/* Get time after sleep as soon as thread weaks up */
	clk_return = clock_gettime(CLOCK_REALTIME, &time_after_sleep);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clk_return != 0) {
		printf("Clock_gettime returned %d\r\n", clk_return);
	}
	double sleptMs = (time_after_sleep.tv_nsec - time_before_sleep.tv_nsec) / (1000.0 * 1000.0);
	printf("SLEPT %fms\r\n", sleptMs);

	}
	return NULL;
}

/* Diese Funktion verwenden um die passende Aufloesung fuer den Millisekunden-Bereich zu erreichen */
void changeSystemTick(unsigned int microsecs) {
	/* Measure time resolution */
	struct _clockperiod get_period;
	int clock_period_return = ClockPeriod_r(CLOCK_REALTIME, NULL, &get_period, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clock_period_return != 0) {
		printf("Clock period error %s\r\n", clock_period_return);
	}
	printf("Clock period before changing is %dus\r\n", (get_period.nsec / (1000)));

	/* Set resolution to given value */
	struct _clockperiod set_period;
	set_period.nsec = microsecs * 1000;
	set_period.fract = 0;
	clock_period_return = ClockPeriod(CLOCK_REALTIME, &set_period, NULL, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clock_period_return != 0) {
		printf("Clock period error %s\r\n", clock_period_return);
	}

	/* Measure time resolution again */
	clock_period_return = ClockPeriod(CLOCK_REALTIME, NULL, &get_period, 0);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}
	if(clock_period_return != 0) {
		printf("Clock period error %s\r\n", clock_period_return);
	}
	printf("Clock period after changing is %dus\r\n", (get_period.nsec / (1000)));
}

int main(int argc, char *argv[]) {
    uint8_t i = 0;
    /* Systemtick auf 10us setzen */
    changeSystemTick(10);
    /* Befolge fuer immer: 2ms schlafen, 2ms arbeiten => 4ms Zykluszeit
       Alle 3 Zyklen Semaphore setzen, auf die Task2 wartet.
    */
    while(1) {
        /* 3. Zyklus => Semaphore setzen */
        if((i % 3) == 0) {
            /* TODO: Semaphore fuer Task2 setzen */
        }
        /* 2ms schlafen */
        runTwoMillisecond();
        /* TODO: Code aus Aufgabe 3 einfuegen, mit dem die Verarbeitungszeit von 2ms realisiert wird */
        i++;
    }
	return EXIT_SUCCESS;
}