#include <stdio.h>

int main() {
    char chave;
    chave = 'h';
    char * p;
    p = &chave;
    char tela;
    tela = *p;
    printf("%c\n", tela);

    p = p + 1;
    printf("%c\n", *p);
    p = 10000; // Endereco absurdo => Segmentation Fault
    printf("%c\n", *p);
}