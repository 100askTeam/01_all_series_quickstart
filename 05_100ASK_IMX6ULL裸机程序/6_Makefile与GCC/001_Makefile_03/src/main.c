#include <stdio.h>
#include "add.h"
#include "sub.h"

int main()
{
	printf("100 ask, add:%d\n", add(10, 10));
	printf("100 ask, sub:%d\n", sub(20, 10));
	return 0;
}
