#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> 
int main(int argc, char **argv)
{
   
    execv("./sequential_min_max.out", argv);
    printf("Beda");
    return 0;    
}