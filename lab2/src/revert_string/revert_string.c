#include "revert_string.h"
#include <string.h>
void swap_revert_string(char *left, char *right)
{
char tmp = *left;
*left = *right;
*right = tmp;
}

void RevertString(char *str)
{
int b=strlen(str);
int a= b/2;
int i;
for (i = 0; i < a; i++)
{
swap_revert_string(&str[i], &str[b - i - 1]);
}
}
