#include <stdio.h>

int cont(int n){
    int s = 0;
    while (n)
    {
        s += n & 1;
        n >>= 1;
    }
    return ((1 << s) - 1) << (18 - s);
}

int main(){
    int value, Nvalue;
    scanf("%d", &value);
    Nvalue = cont(value);
    for (int i = 0; i < 8; i++)
    {
        putchar(Nvalue & (1 << (7 - i)) ? '1' : '0');
    }
    putchar('\n');
    return (0);
}