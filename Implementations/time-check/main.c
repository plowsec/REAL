#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <string.h>
#include <openssl/md5.h>

#define MAX_BUFFER_LEN 4096

#define TIME_CHECK false
enum Bool   {
    false = 0,
    true=1
};
typedef enum Bool Bool;
int RDTSC(void) {
    int x;
    __asm__ volatile ("rdtsc" : "=A"(x));
    return x;
}

/*int debuggerpresent()   {
    int present=0;
    __asm__ volatile ("movl %%fs:48, %%eax\n\t"
                      "movb 2(%%eax),%%al\n\t"
                      "movb %%al, %0\n\t":"=r"(present));
                     
    printf("prsent : %d\n", present);
    return 0;
}*/
int main(int argc, char *argv[])
{
    unsigned int time1 = 0, time2 = 0;
    int i = 0;

    //encrypted password
    unsigned char passwd[14] = {178,205,159,153,179,146,185,163,174,147,132,164,204,142};

    //basic anti-debug
    if(ptrace(PTRACE_TRACEME,0,1,0)==-1)    {
        printf("Leave me alone!\n");
        return -1;
    }

   // debuggerpresent();
    printf("Performances test about to begin...\n");
    time1 = RDTSC();
    printf("Test 1 ! \n");
    time2 = RDTSC();
    printf("First test took : %d\n",time2-time1);

    time1 = RDTSC();
    printf("Test 2 ! \n");
    time2 = RDTSC();
    printf("Second test took : %d\n",time2-time1);

    //the difference between two successiv calls of rdtsc should be huge if the process is launched is a debugger
    time1 = RDTSC();
    time2 = RDTSC();
    printf("Time Check (debug): %d\n", time2-time1);

    if(time2-time1 > 300)  {
        printf("Leave me alone (2)\n");
        printf("Debugger detected by Time Check : %d\n", time2-time1);
        return -1;
    }

    //did the user forgot to give a password as parameter?
    if(argc<2)  {
        printf("Usage : ./crackme password\n");
        return 0;
    }

    //we put this here to simplify the code below
    if(strlen(argv[1])!=14)   {
        printf("Pass incorrect\n");
        return -1;
    }

    for(i=0;i<14;i++)  {
        time1 = RDTSC();

        //each character is xored with 0xFC and compared to the corresponding character of the real encrypted password
        if((argv[1][i]^0xfc)!=passwd[i])  {
            printf("Pass incorrect (2)\n");
            return -1;
        }

        //because an analyst will want to put a breakpoint here, let's annoy him
        time2 = RDTSC();
        if(time2-time1>1000)    {
            printf("You really need to leave me alone! \nDetected by Time Check (2) : %d\n",time2-time1);
            return -1;
        }
        printf("Time check (2) (debug) : %d\n", time2-time1);
    }

    printf("The pass you entered is correct\n");
    return 0;
}
