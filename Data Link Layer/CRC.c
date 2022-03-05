#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main() {

    char array[4]={'b','b','b','\0'};
    char gpstr[20];
    unsigned long gp=0;
    unsigned long mask=0;
    printf("%s\n",array);
    unsigned long n = 0;	/* 60 = 1100 1000 */
    int i;
    for(i=0;i<4;i++)
    {
        n=n<<8|array[i];
    }
    int c,k;
    for (c = 63; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    char retrieved[4];
    for(i=3;i>=0;i--)
    {
        retrieved[i]=n|0;
        n>>8;
    }
    printf("%s\n",array);
    scanf("%s",&gpstr);
    printf("%s",gpstr);
    for(i=0;i<strlen(gpstr);i++)
    {
        if(gpstr[i]=='1')
        {
            gp=gp|(1<<(strlen(gpstr)-i-1));
        }
        mask=mask|(1<<i);
    }
    printf("\nGenerator polynomial :\n");
    for (c =63; c >= 0; c--)
    {
        k = gp >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    printf("Dividend %d\n",n);
    printf("After appending 0 dividend\n");
    n=n<<(strlen(gpstr)-1);
    for (c = 63; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    unsigned long temp=(n>>31)&mask;
    long q=0;
    for(i=30;i>=0;i--)
    {
        if(temp>>(strlen(gpstr)-1)==1)
        {
            q=q|1<<i;
            temp=temp^gp;
        }
        temp=temp<<1;
        temp=temp|((n>>i)&1);
    }
    printf("Remainder:\n");
    for (c =63; c >= 0; c--)
    {
        k = temp >> c;
        if (k & 1)
            printf("1");
        else
            printf("0");
    }
        printf("\n");
    printf("Quotient:\n");
    for (c =63; c >= 0; c--)
    {
        k = q >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    n=n^temp;
    temp=(n>>31)&mask;
    q=0;
    printf("After appending remainder: \n");
    for (c = 63; c >= 0; c--)
    {
        k = n >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");
    for(i=30;i>=0;i--)
    {
        if(temp>>(strlen(gpstr)-1)==1)
        {
            q=q|1<<i;
            temp=temp^gp;
        }
        temp=temp<<1;
        temp=temp|((n>>i)&1);
    }

    temp=temp>>1;
    printf("Remainder:\n");
    for (c =63; c >= 0; c--)
    {
        k = temp >> c;
        if (k & 1)
            printf("1");
        else
            printf("0");
    }
        printf("\n");
    printf("Quotient:\n");
    for (c =63; c >= 0; c--)
    {
        k = q >> c;
        if (k & 1)
          printf("1");
        else
          printf("0");
    }
    printf("\n");




}

