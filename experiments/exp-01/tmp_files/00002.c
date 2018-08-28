#include<stdio.h>

int main() {
#pragma nounroll
	for(int i=0; i < 5; i++)
		printf("%d\t",i);
}
