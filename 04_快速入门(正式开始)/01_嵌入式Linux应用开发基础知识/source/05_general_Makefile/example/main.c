#include <stdio.h>

extern void sub_fun(void);
extern void sub2_fun(void);
void sub3_fun(void);

int main(int argc, char* argv[])
{
    printf("Main fun!\n");
    sub_fun();
    sub2_fun();
    sub3_fun();
    return 0;
}
