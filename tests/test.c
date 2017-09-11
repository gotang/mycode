#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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

int main()
{
	int lines = 0;
	if(scanf("%d", &lines) != EOF) {
		printf("lines:%d\n", lines);
		int n = 0;
		for (int i = 0; i < lines; i++) {
			scanf("%d", &n);
			int divisorNums = 1;
			for (int k = 2; k <= n; k++) {
				int num = 0;
				while (n % k == 0) {
					n /= k;
					num++;
				}
				divisorNums *= (num + 1);
			}
			printf("%d\n", divisorNums);
		}
		printf("%d\n", INT_MAX);
	}
	return 0;
}
