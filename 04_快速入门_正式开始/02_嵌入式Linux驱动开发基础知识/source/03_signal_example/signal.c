#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void my_sig_func(int signo)
{
    printf("get a signal : %d\n", signo);
}

int main(int argc, char **argv)
{
    int i = 0;
    
    signal(SIGIO, my_sig_func);
    
    while (1) 
    {
        printf("Hello, world %d!\n", i++);
        sleep(2);
    }
    
    return 0;
}


