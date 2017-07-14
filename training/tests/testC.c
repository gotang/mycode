#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//how many 1
int _1s(int x) {
	int countx = 0;
	while (x) {
		countx++;
		x = x & (x - 1);
	}
	return countx;
}

int _0s(int x) {
	int countx = 0;
	while (x) {
		countx++;
		x = x | (x + 1);
	}
	return countx;
}

int main() {
	int x = 1234;
	char *s = (char*)&x;
	for(int i = 0; i < 4; i ++) {
		printf("x is %d\n", (int)(*(s + i)));
	}
	return 0;
}
