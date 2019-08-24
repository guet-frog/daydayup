#include <stdio.h>
#include <string.h>

#define MAX_LEN    5000
#define DEBUG_INFO

char g_inputBuff[MAX_LEN * 2];


//int main(int argc, char **argv)
int main(void)
{
    char curCh;
    unsigned int i = 0;
    
    unsigned int lastBlackWrite = 0;
    
    scanf("%[^\n]", g_inputBuff);
#ifdef DEBUG_INFO
    printf("%s    ", g_inputBuff);
    printf("strlen(g_inputBuff) = %d    ", strlen(g_inputBuff));
#endif
    if (strlen(g_inputBuff) > MAX_LEN)
    {
        printf("max len < 5000\n");
        return -1;
    }
    
    for (i = 0; i < strlen(g_inputBuff); i++)
    {
        if (' ' == *(g_inputBuff + i))
        {
            lastBlackWrite = i;
        }
    }
#ifdef DEBUG_INFO
    printf("lastBlackWrite = %d    ", lastBlackWrite);
    printf("i = %d    ", i);
#endif
    
    printf("%d\n", (i - lastBlackWrite));
    
    return 0;
}