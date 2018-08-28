#include<stdio.h>

int main() {
#pragma unroll
	for(int i=0; i < 5; i++)
		printf("%d\t",i);
}
