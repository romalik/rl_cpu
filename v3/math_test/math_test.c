#include <stdio.h>


void printBin(int c) {
	printf("%d:\t",c);
	for(int i = 15; i>=0; i--) {
		printf("%s",(c & (1<<i))?"1":"0");
	}
	
	printf("\n");

}

int main() {
printBin(4);
printBin(10);
printBin(15);
printBin(65535);


printBin(32767);
printBin(32768);
printBin(-32767);
printBin(-32768);
printBin(-32769);
return 0;	

}
