#include <stdio.h>
#include <sub2.h>

void sub2_fun(void)
{
    printf("Sub2 fun, B = %d!\n", B);   
#ifdef DEBUG
	printf("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
}
