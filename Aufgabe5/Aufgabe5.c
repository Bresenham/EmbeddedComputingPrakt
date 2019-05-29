#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


int main (void) {

	int fd;
	int size_written;
	char led1 = 0x01;
	char led2 = 0x02;
	char led3 = 0x04;
	char led4 = 0x08;

	//XOR
	char led2_shift = led2 >> 1;
	led3 = (led1^led2) << 2;

	//OR
	led2_shift = led2 >> 1;
	led4 = (led1|led2) << 3;

	char toWrite;
	toWrite = led1 | led2 | led3 | led4;

	//open sets Erno
	fd = open("/dev/leds", O_RDWR);
	if(errno != EOK) {
		printf("Error: %s\r\n", strerror(errno));
		exit(-1);
	}

	//write sets erno
	size_written = write(fd, toWrite, sizeof(toWrite));
	if (size_written != sizeof(toWrite) ) {
		printf( "Error writting to /dev/leds: %s\r\n", strerror(errno) );
		return -1;
	}


	// close the file
	close(fd);


}
