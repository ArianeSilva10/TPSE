#include <stdio.h>
#include <stdbool.h>
int setBits(bool w, unsigned int i, unsigned int j, unsigned int value){
    if(i > 31 || j > 31 || i > j){ //verifica se se i e j vão ser válidos para uso
        return -1;
    }
    unsigned int  mask = ~((1 << (j - i + 1) - 1) << i);
    unsigned int res = value & mask;
    unsigned int ww = (w ? ~0: 0) & (((1 << (j - i + 1)) - 1) << i);
    res |= ww;
    return res;
}
int main(){
    unsigned int I,J,VALUE, b;
    bool W;
    int RES;
    scanf("%d%u%u%u", &b, &I, &J, &VALUE);
    W = b;
    RES = setBits(W, I, J, VALUE);
    printf("%d\n", RES);
}