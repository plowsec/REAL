#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static int debugger_present = -1;
static void sigtrap_handler(int signo)
{
    debugger_present = 0;
    signal(SIGTRAP, SIG_DFL);
}
void debug_break(void)
{
    debugger_present = 1;
    signal(SIGTRAP, sigtrap_handler);
    raise(SIGTRAP);
}
int main()  {

    debug_break();

    if(debugger_present==0)    {

        //insert the important code here
        printf("if you see this, you are a good boy\n");
        printf("quitting kindly...\n");
        return 0;
    }
    
    //debugger detected, insert appropriate code here
    printf("If you see this, you are a very bad boy.\n");
    return -1;
}

