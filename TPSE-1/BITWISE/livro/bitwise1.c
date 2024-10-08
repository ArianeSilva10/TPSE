/* Imprimindo em bits um inteiro unsigned */
#include <stdio.h>

int main()
{

    unsigned x;
    void exibeBits(unsigned);
    printf("Digite um inteiro unsigned: ");
    scanf("%u", &x);
    exibeBits(x);
    return 0;
}
void exibeBits(unsigned valor)
{
    unsigned c, exibeMascara = 1 << 15;
    printf("%u = ", valor);
    for (c = 1; c <= 16; c++)
    {
        putchar(valor & exibeMascara ? '1' : '0');
        valor <<= 1;

        if (c % 8 == 0)
            putchar(' ');
    }
    putchar('\n');
}
/*
2 = 00000000 00000010
exibeMascara = 10000000 00000000
deslocamentos: 00000000 00000100
               00000000 00001000
               00000000 00010000
               00000000 00100000
               00000000 01000000
               00000000 10000000
               00000001 00000000
               00000010 00000000
               00000100 00000000
               00001000 00000000
               00010000 00000000
               00100000 00000000
               01000000 00000000
               10000000 00000000
               00000000 0000000
2 & exibeMascara = 00000000 000000010
*/