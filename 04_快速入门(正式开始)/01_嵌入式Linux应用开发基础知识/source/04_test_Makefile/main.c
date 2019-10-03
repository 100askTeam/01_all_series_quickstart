#include <stdio.h>

extern void sub_fun(void);

int main(int argc, char* argv[])
{
    printf("Main fun!\n");
    sub_fun();
    return 0;
}
