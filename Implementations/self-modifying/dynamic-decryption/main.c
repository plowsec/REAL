#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <unistd.h>

unsigned char shellcode[] __attribute__((aligned(4096)))="\xeb\x16\x5e\x31\xd2\x52\x56\x89\xe1\x89\xf3\x31\xc0\xb0\x0b\xcd"
    "\x80\x31\xdb\x31\xc0\x40\xcd\x80\xe8\xe5\xff\xff\xff\x2f\x62\x69"
        "\x6e\x2f\x73\x68";
unsigned char code[] __attribute__((aligned(4096)))="";
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


    //ce shellcode n'est pas chiffré, c'est juste pour tester
    int shell[]={137,229,83,131,236,36,198,69,218,178,198,69,219,205,198,69,220,159,198,69,221,153,198,69,222,179,198,69,223,146,198,69,224,185,198,69,225,163,198,69,226,174,198,69,227,147,198,69,228,132,198,69,229,164,198,69,230,204,198,69,231,142,199,69,240,252,0,0,0,199,69,244,0,0,0,0,199,69,236,0,0,0,0,199,69,232,0,0,0,0,106,0,106,1,106,0,106,0,232,40,254,255,255,131,196,16,131,248,255,117,26,131,236,12,104,16,136,4,8,232,195,253,255,255,131,196,16,184,255,255,255,255,233,210,0,0,0,131,236,12,255,117,8,232,203,253,255,255,131,196,16,131,248,14,116,26,131,236,12,104,32,136,4,8,232,150,253,255,255,131,196,16,184,255,255,255,255,233,165,0,0,0,199,69,244,0,0,0,0,235,110,15,49,137,69,236,139,85,244,139,69,8,1,208,15,182,0,15,190,192,51,69,240,137,194,141,77,218,139,69,244,1,200,15,182,0,15,182,192,57,194,116,23,131,236,12,104,51,136,4,8,232,73,253,255,255,131,196,16,184,255,255,255,255,235,91,15,49,137,69,232,139,69,232,43,69,236,61,220,5,0,0,126,23,131,236,12,104,76,136,4,8,232,32,253,255,255,131,196,16,184,255,255,255,255,235,50,131,69,244,1,139,93,244,131,236,12,255,117,8,232,36,253,255,255,131,196,16,57,195,15,130,121,255,255,255,131,236,12,104,111,136,4,8,232,236,252,255,255,131,196,16,184,0,0,0,0,139,93,252,201,195};
    int i = 0;
    //int key = 0xFC;

    //conversions de la suite de codes décimaux de la variable shell en une string format shellcode "\x56\xdf"...etc
    while(i < sizeof(shell)/sizeof(shell[0]))  {
        int nb = shell[i];
        char hex[4];
        char opcode[6]="\\x";

        sprintf(hex,"%02x",nb);     
        strcat(opcode, hex);
        strcat(code, opcode);

        i++;
    }

    //on vérifie que tout s'est bien passé :
    //printf("Shellcode : %s\n",code);
    
    /*void *page = (void *) ((*shellcode) &~(getpagesize() - 1));
    if(mprotect(page, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC)==-1)   {
        printf("mprotect failed\n");
        return -1;
    }*/

    if(mprotect(&code, sizeof(code), PROT_READ|PROT_EXEC|PROT_WRITE))
    {
        printf("mprotect failed\n");
        perror("erreur");
        return -1;
    }
    //int (*func)() = code;
    //seg fault -.-
    //int (*checkPassword)();

    //checkPassword = (int(*)())shellcode;
    
    //je rajouterai l'argument pour transmettre le mot de passe une fois que l'exécution du shellcode sera fonctionnelle
    //(int)(*checkPassword)();
    //
    ((void (*)())code)();

   /* __asm__ volatile    (
            "leal $0,%%eax\n\t"
            "call *%%eax\n\t"
            :
            :"g"(shellcode)
            );*/


    return 0;
}
