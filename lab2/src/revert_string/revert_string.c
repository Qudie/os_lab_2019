#include "revert_string.h"
#include <string.h>
void RevertString(char *str)
{
    int b=strlen(str);
    char a[b];
    char a1[b];
    int i;
    for(i=0;i<b;i++)
    {a1[i]=str[i];}
    for(i=0;i<b;i++)
    {a[i]=a1[b-i-1];}
    str = a;
}

