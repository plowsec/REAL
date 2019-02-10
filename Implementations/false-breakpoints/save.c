#include <stdio.h>
#include <signal.h>
void handler(int signo){}

int main()  {
    signal(SIGTRAP,handler);
    __asm__("int3");
    printf("hello\n");
}
