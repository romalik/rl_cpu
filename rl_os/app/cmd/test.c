#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv) {
	int fd;  
	int ch[100];
	int n;
	int * c;

	mkfifo("/a", 0777);
	fd = open("/a", O_RDONLY);

	yield();

	while(1) {
		n = read(fd, ch, 100);
		if(!n) break;
		c = ch;
		printf("[test] Recv %d : ", n);
		while(n) {
			putchar(*c);
		  n--; 
		  c++;
		}
	}
	return 0;
}
