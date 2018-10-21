#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char ** argv) {
	int fd;
	int fd2;
	uint16_t src;
	uint16_t target;
	if(argc < 3) {
		printf("swab infile outfile\n");
		return 1;
	}
	fd = open(argv[1], O_RDONLY);
	fd2 = open(argv[2], O_CREAT | O_WRONLY);
	
	while(read(fd, &src, 2)) {
		target = ((src & 0xff00) >> 8) | ((src & 0x00ff) << 8);
		write(fd2, &target, 2);
	}
	close(fd);
	close(fd2);
	return 0;
}
	
