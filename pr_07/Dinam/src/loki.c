#include"loki.h"
#include"stdlib.h"
#include"stdio.h"
#include"time.h"
static int prisoned(int check)
{
    if(check == 1)
    {
        return 1;
    }
    return 0;
}

void heyAsLoki()
{
    srand(time(NULL));

    int k = rand();
    if(prisoned(k%2)==0)
    {
        printf("%s","can`t speak couse afraid of Tor\n");
    }
    else
    {
        printf("%s","don`t wan`t speak with you*\n");
    }
}