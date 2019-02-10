#include <stdio.h>
#include <string.h>
int strcmp(const char *str1, const char *str2)  {
    printf("Call to strcmp intercepted] str1 = %s\n",str1);
    printf("str2 = %s\n",str2);
    return 0;
}
