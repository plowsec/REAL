#include <stdio.h>
#include <string.h>
int main (int argc, char *argv[])   {
    if(argc<2)  {
        printf("Usage : ./patchme motdepasse\n");
        return 0;
    }
    if(strcmp(argv[1],"1234")==0)   {
        printf("%s","Mot de passe correct\n");
        return 0;
    }
    printf("%s", "Mot de passe incorrect\n");
    return 1;
}
