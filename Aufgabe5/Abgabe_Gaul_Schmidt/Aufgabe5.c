#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

int main (void) {
	while(true) {
		uint8_t buf[2];
		int read_bytes = 0;
		while( (read_bytes = read(STDIN_FILENO, &buf, sizeof(buf))) < 0 && errno == EINTR )
			;
		if(errno != EOK) {
			printf("Error reading stdin %s\r\n", strerror(errno));
			exit(-1);
		}
		if(read_bytes == 2) {
			printf("Read %d bytes\r\n", read_bytes);
			uint8_t led0 = (buf[0] - '0');
			uint8_t led1 = (buf[1] - '0');
			uint8_t led2 = (led0 ^ led1);
			uint8_t led3 = (led0 | led1);
			const char toWrite = (led3 << 3) | (led2 << 2) | (led1 << 1) | led0;

			//open sets Erno
			const int fd = open("/dev/leds", O_RDWR);
			if(errno != EOK) {
				printf("Error @ open: %s\r\n", strerror(errno));
				exit(-1);
			}

			//write sets erno
			const int size_written = write(fd, &toWrite, sizeof(toWrite));
			if (size_written != sizeof(toWrite) ) {
				printf("Error writting to /dev/leds: %s\r\n", strerror(errno));
				return -1;
			}
			// close the file
			close(fd);
			if(errno != EOK) {
				printf("Error @ close: %s\r\n", strerror(errno));
				exit(-1);
			}

		}
	}

	return EXIT_SUCCESS;
}
