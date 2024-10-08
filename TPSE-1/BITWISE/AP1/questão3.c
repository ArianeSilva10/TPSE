#include <stdio.h>
#include <stdbool.h>

int countBits(bool w,long int value){
    int count = 0;
    for(int i = 0; i < 32; i++){ //laço for para lidar como os 32 bits
        if(w == 1){
            count += value & 1;
        } else{
            count += !(value & 1);
        }
        value >>= 1;
    }
    return count;
}

int main(){
    long int v;
    int num, b;
    bool W;
    scanf("%d%ld", &b, &v);
    W = (b == 1)? true : false;
    num = countBits(W, v);
    printf("%d\n", num);
}