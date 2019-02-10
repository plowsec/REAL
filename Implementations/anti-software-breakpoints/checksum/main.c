#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include "md5.c"
#define MAX_FILENAME_LEN 1024
#define MAX_BUFFER_LEN 4096

char *str2md5(const char *str, int length) {

    /*printf("coucou\n");

    MDString(str);
    printf("coucou\n");*/
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5Update(&c, str, 512);
        } else {
            MD5Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}
unsigned int checksum(unsigned char *binBytes, size_t size)
{
    unsigned char *buf = (unsigned char *)binBytes;
    size_t i;
    unsigned int cs = 0;
    for(i=0;i<size;++i)
        cs+=(unsigned int)(*buf++);
    return cs;
}

int getFileName(char **fileName) {
    *fileName = (char*) malloc(MAX_FILENAME_LEN);
    if (fileName == NULL)
    {
        printf("Allocation failed\n");
        return -1;
    }
    memset(*fileName,0, MAX_FILENAME_LEN);

    //on récupère le lien symbolique vers notre exécutable (Linux)
    if(readlink("/proc/self/exe",*fileName,MAX_FILENAME_LEN-1)<0)
    {
        printf("Read filename failed\n");
        return -1;
    }

    return 0;
}
size_t getBytes(char *fileName, unsigned char **binBytes)    {
    FILE *file = NULL;
    size_t fileLength = 0;

    file = fopen(fileName,"rb");
    if(!file)   {
        printf("no such file\n");
        return -1;
    }
    //get file length
    fseek(file,0,SEEK_END);
    fileLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    *binBytes = (unsigned char*)malloc(fileLength);
    if(!*binBytes)  {
        printf("Malloc failed\n");
        return -1;
    }

    fread(*binBytes,sizeof(*binBytes),fileLength,file);
    fclose(file);

    return fileLength;
}

int main (int argc, char *argv[])
{
    //strong anti-debug
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
    {
        printf("Leave me alone!\n");
        return -1;
    }

    //detect if a password was given as parameter
    if(argc<2)  {
        printf("Usage : ./patchme password\n");
        return 0;
    }

    unsigned char *binBytes = NULL;
    char *fileName = NULL;
    size_t fileLength = 0;
    unsigned int selfChecksum = 0;
    char *mdp=(char*)malloc(MAX_BUFFER_LEN);

    //get the path to our own binary
    getFileName(&fileName);
    //printf("debug filename : %s\n",fileName);

    //get the content of the given file
    fileLength=getBytes(fileName, &binBytes);

    //compute checksum of binary
    selfChecksum = checksum(binBytes,fileLength);
    //printf("checksum : %#x\n", selfChecksum);

    selfChecksum+=528;

    //compute hash
    sprintf(mdp,"%d", selfChecksum);
    mdp= str2md5(mdp,strlen(mdp));
    //printf("hash : %s\n", mdp);

    if(strcmp(argv[1], mdp)==0)   {
        printf("The pass you entered was correct, here is the key : %s\n",mdp);
    }
    else    {
        printf("%s", "Password incorrect\n");
    }

    //clean up
    if(binBytes != NULL)
        free(binBytes);
    if(fileName!=NULL)
        free(fileName);
    if(mdp!=NULL)
        free(mdp);

    return 1;
}
