#include <stdio.h>

int main(){
    long int n;
    scanf("%ld", &n);
    int mask = 0xF;
    for (int i = 7; i >= 0; i--)
    {
        long int bits = (n >> (i*4)) & mask;
        for (int j = 0; j < 4; j++)
        {
            printf("%ld", (bits >> j) & 1);
        }
    printf("\n");
    }
    return (0);
}