#include <stdio.h>

unsigned int buf[1024];

int main() {
	size_t n = 0;
	while(n = read(0, buf, 1024)) {
		write(1, buf, n);
	}
	return 0;
}
