#include "revert_string.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

void RevertString(char *str)
{
    int len = strlen(str)-1;
    char *new_str = malloc(sizeof(char) * len);
    int i;
    for(i=0; i<strlen(str);i++, len--)
    {
        new_str[i] = str[len];
        printf("%c %d %d\n",new_str[i], i, len);
    }
    for(i=0; i<strlen(str); i++)
    {
        str[i] = new_str[i];
    }
    //printf("%c  sd",new_str[1]);
}

