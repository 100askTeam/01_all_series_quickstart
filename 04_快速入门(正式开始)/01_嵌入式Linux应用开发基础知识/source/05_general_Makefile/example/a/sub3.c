#include <stdio.h>
#include <sub3.h>

void sub3_fun(void)
{
    printf("Sub3 fun, C = %d!\n", C);
	
#ifdef DEBUG
		printf("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif

#ifdef DEBUG_SUB3
		printf("It is only debug info for sub3.\n");
#endif

}
