#include <stdio.h>

int main() {
    char chave;
    chave = 'h';
    char cofre;
    cofre = 't';
    char * p;
    p = &chave;
    printf("%c\n", *p);
    p = p + 1;
    printf("%c\n", *p);
    p = 10000;
    printf("%c\n", *p);
}