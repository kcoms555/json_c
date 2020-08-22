#include <stdio.h>
static void f1(void){
	printf("f1\n");
}
void f2(void){
	printf("f2\n");
	f1();
}
