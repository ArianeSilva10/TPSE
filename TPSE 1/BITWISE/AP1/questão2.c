#include <stdio.h>

int mov(int n){
    int s = 0;
    while(n){
        s += n & 1;
        n >>= 1;
    }
    return ((1 << s) - 1) << (9 - s);
}

int main(){
    int n;
    scanf("%d", &n);
    int res = mov(n);
    int mask = 1 << 8;
    for(int i = 0; i < 8; i++){
        putchar(res & mask? '1' : '0');
        res <<= 1;
    }
    putchar('\n');
    return (0);
}