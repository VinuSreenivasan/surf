#include<stdio.h>

int main() {
#pragma clang loop id(myloop)
	for(int i=0; i < 5; i++)
		printf("%d\t",i);
}
