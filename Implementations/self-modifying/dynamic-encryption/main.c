#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <unistd.h>
int encrypt(unsigned long addr, unsigned long length)
{

    //make the section of the function to encrypt writable
    void *page = (void *) ((addr) &~(getpagesize() - 1));
    if(mprotect(page, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC)==-1)   {
        printf("mprotect failed\n");
        return -1;
    }
    /*if(mprotect( (void*)  ((addr) &~ 0xFFF),(length) + ((addr) - ((addr) &~ 0xFFF)),PROT_READ | PROT_EXEC | PROT_WRITE)==-1)   {
        printf("mprotect failed\n");
        return -1;
    }*/

    //encryption routine
    __asm__ volatile ("pushl %%eax\n\t"
                      "pushl %%ecx\n\t"
                      "movl %0, %%ecx\n\t"
                      "add %1, %%ecx\n\t"
                      "movl %2, %%eax\n\t"
                      "enc_loop%=:\n\t"
                      "xorb $0xFC,%%ds:(%%eax)\n\t"
                      "inc %%eax\n\t"
                      "cmp %%ecx,%%eax\n\t"
                      "jl enc_loop%=\n\t"
                      "pop %%eax\n\t"
                      "pop %%ecx"
                      :
                      :"g"(addr),"g"(length),"g" (addr)
                      :"ebx","edx");
    return 0;
}

//the function we will test our encryption thing on
int checkPassword(char *mdp)
{
    unsigned char passwd[14] = {178,205,159,153,179,146,185,163,174,147,132,164,204,142};
    unsigned int key = 0xFC;
    int i = 0, time1 = 0, time2 = 0;

    //basic anti-debug
    if(ptrace(PTRACE_TRACEME,0,1,0)==-1)    
    {
        printf("Leave me alone!\n");
        return -1;
    }

    if(strlen(mdp)!=14) 
    {
        printf("Password incorrect\n");
        return -1;
    }

    //password verification routine
    for(i=0;i<strlen(mdp);i++)    
    {
        __asm__ volatile ("rdtsc" : "=A"(time1));

        if((mdp[i]^key)!=passwd[i])    
        {
            printf("Password incorrect (2)\n");
            return -1;   
        }

        __asm__ volatile ("rdtsc" : "=A"(time2));

        //because this piece of code is the key to crack the program, let's annoy anyone who wants to put a breakpoint or single steps his way down there.
        if(time2-time1>1500)    {
            printf("You really need to leave me alone!\n");
            return -1;
        }
    }
    printf("Password correct, GG\n");
    return 0;
}
void blup()	{
	printf("coucou\n");
}

//declaration of a stub function to mark the end of the previous function
void stub(){}

int main(int argc, char *argv[])
{
    //in case the user forgot to give the password
    if(argc<2)  {
        printf("Usage : ./crackme password\n");
        return -1;
    }

    if(strlen(argv[1])<=3)  {
        printf("Please enter a valid password\n");
        return -1;
    }

    unsigned long functionLength = 0;
    char *startAddress, *endAddress = NULL;

    //get the addresses of the functions
    startAddress = (char *)&blup;
    endAddress = (char *)&stub;

    functionLength = endAddress - startAddress;
    
    blup();

    //encrypt
    encrypt((unsigned long) startAddress, functionLength);

    //decrypt (xor...)
    encrypt((unsigned long) startAddress, functionLength);

    //if the encryption/decryption went well, the function must still be usable.
    blup();
    return 0;
}
