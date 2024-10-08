#include <stdio.h>

int cont(int n){
    int bit = 0;
    while(n){
        bit += n & 1;
        n >>= 1;
    }
    return bit;
}

int main(){
    int num;
    scanf("%d", &num);
    int res = cont(num);
    printf("%d\n", res);
    return (0);
}