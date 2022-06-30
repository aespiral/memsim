#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/***********************************************
 * Representacao da memoria
 *   A memoria eh um array de celulas
 *   Cada celula tem endereco, tipo e conteudo; e pode ter nome
 *   O conteudo pode ser inteiro, float, char
 */

#define TAM_MEM 256
#define END_INI 4000
#define TAM_PROG 16

/*
 * Valores podem ser de 3 tipos
 * Exemplos:
 * |  struct valor v1, v2, v3
 * |  v1.mod = VAL_INT;
 * |  v1.i = 42;
 * |  v2.mod = VAL_FLOAT;
 * |  v2.f = 3.14;
 * |  v3.mod = VAL_CHAR;
 * |  v3.c = 'p';
 * 
 *  v1 representa 42
 *  v2 representa 3.14
 *  v3 representa p
 */
#define VAL_INT 1
#define VAL_FLOAT 2
#define VAL_CHAR 3
struct valor {
    int mod;
    int i;
    float f;
    char c;
};

// Representacoes dos tipos (campo tip)
/*
 * Representacao  de uma celula de memoria com os seguintes campos:
 * - endereco
 * - conteudo
 * - tipo do conteudo
 * - nome (opcional)
 * 
 *  |   struct memcel m1, m2;
 *  |   m1.end = 4097;
 *  |   m1.con = v1
 *  |   m1,tip = INT;
 *  |   m2.end = 4098;
 *  |   m2.con = v2;
 *  |   m2.tip = FLOAT;
 *  |   strcpy(m2.nom, "pi")
 * 
 *  m1 e m2 representam as celulas:
 *  +------+-------+-------+--------+
 *  | 4097 | 42    | INT   |        |
 *  +------+-------+-------+--------+
 *  | 4097 |  3.14 | FLOAT | pi     |
 *  +------+-------+-------+--------+
 */
#define LIVRE 0
#define INT 1
#define FLOAT 2
#define CHAR 3
#define INT_PTR 4
#define FLOAT_PTR 5
#define CHAR_PTR 6
struct memcel {
    int end;           // ENDereco
    struct valor con;  // CONteudo
    int tip;           // TIPo
    char nom[256];     // NOMe
};

/*
 * MEMORIA
 *  como variavel global
 */
struct memcel mem[TAM_MEM];


/*************************************************
 * Representacao de programas em C (miniC)
 *  Instrucoes representadas
 *  - Declaracao de variaveis
 *  - Atribuicao
 */

/*
 * Variaveis sao representadas pelo nome e pelo modo de acesso
 * 
 * Exemplos:
 * |  struct acessa_variavel t1, t2, t3;
 * |  t1.mod = END_VAR;
 * |  strcpy(t1.nom,"x");
 * |  t2.mod = VAR;
 * |  strcpy(t2.nom,"y");
 * |  t3.mod = DESREF_VAR;
 * |  strcpy(t3.nom,"w");
 * 
 * t1 representa &x
 * t2 representa y
 * t3 representa *w
 */
#define END_VAR 1
#define VAR 2
#define DESREF_VAR 3
struct acessa_variavel {
    int mod;
    char * nom;
};

/*
 * Declaracoes sao representadas pelo tipo e pelo modo de acesso
 * >> Apenas os modos de acesso VAR e DESREF_VAR sao validos
 * Exemplos:
 * |   struct declaracao d1, d2, d3;
 * |   d1.var = t2;
 * |   d1.tip = FLOAT;
 * |   d2.var = t3;
 * |   d2.tip = INT;
 * |   struct acessa_variavel t4;
 * |   t4.mod = DESREF_VAR;
 * |   strcpy(t4.nom, "a");
 * |   d3.var = t4;
 * |   d3.tip = CHAR;
 * 
 * d1 representa float y
 * d2 representa int *w
 * d3 representa char *a
 */
struct declaracao {
    struct acessa_variavel var;
    int tip;
};

/*
 * Atribuicoes sao representadas por dois campos:
 * - lhs (left handed side/ lado esquerdo)
 *      >> acesso a variavel, pode ser VAR ou DESREF_VAR
 * - rhs (riht handed side/ lado direito)
 *      >> pode ser acesso a variavel ou valor
 *  Exemplo:
 *  |    struct atribuiicao a1, a2;
 *  |    a1.mod = ATRIB_VAL;
 *  |    a1.lhs = t2;
 *  |    a1.rhs_val = v1;
 *  |    a2.mod = ATRIB_VAR;
 *  |    a2.lhs = t3;
 *  |    a2.rhs_var = t4;
 * 
 *  a1 representa y = 42
 *  a2 representa *w = *a
 */
#define ATRIB_VAL 1
#define ATRIB_VAR 2
struct atribuicao {
    int mod;
    struct acessa_variavel lhs;
    struct valor rhs_val; 
    struct acessa_variavel rhs_var;
};

#define STNC_DEC 1
#define STNC_ATR 2
struct sentenca {
    int mod;
    struct declaracao dec;
    struct atribuicao atr;
};

/*
 * PROGRAMA
 * Como variavel global
 */ 
struct sentenca programa[TAM_PROG];

/**********************************************************
 * Pretty Print da memoria
 */
void pprint_mem(int comeco, int fim) {
    char* barra = "+======+=======+===========+==--...";
    puts(barra);
    puts("| END  | VALOR | TIPO      | NOME");
    puts(barra);

    int i;
    char print_valor[256];
    char print_tipo[256];
    for(i = comeco - END_INI; i <= fim - END_INI; i = i + 1) {
        switch(mem[i].con.mod){
            case VAL_INT:   
                sprintf(print_valor, "%5d", mem[i].con.i); 
                break;
            case VAL_FLOAT: 
                sprintf(print_valor, "%f", mem[i].con.f);
                break;
            case VAL_CHAR:  
                sprintf(print_valor, "%c    ", mem[i].con.c);
        }
        switch(mem[i].tip){
            case LIVRE:
                strcpy(print_tipo,"         ");
                break;
            case INT:
                strcpy(print_tipo,"INT      ");
                break;
            case INT_PTR:
                strcpy(print_tipo,"INT_PTR  ");
                break;
            case FLOAT:
                strcpy(print_tipo,"FLOAT    ");
                break;
            case FLOAT_PTR:
                strcpy(print_tipo,"FLOAT_PTR");
                break;
            case CHAR:
                strcpy(print_tipo,"CHAR     ");
                break;
            case CHAR_PTR:
                strcpy(print_tipo,"CHAR_PTR ");
                break;                
        }

        printf("| %4d | %s | %s | %s\n", mem[i].end, print_valor, print_tipo, mem[i].nom);
    }
   puts(barra);

}

/**********************************************************
 * Executa o programa
 */
void exec_dec(struct declaracao dec) {

    // Busca uma celula livre
    int i;
    for(i = 0; i < TAM_MEM; i = i + 1) {
        if (mem[i].tip == LIVRE) {
            break;
        }
    }
    if (i == TAM_MEM) {
        printf("ERRO: estouro da memória simulada!!\n");
        exit(1);
    }

    // Preenche o tipo e o nome da celula
    mem[i].tip = dec.tip;
    strcpy(mem[i].nom, dec.var.nom);
}

void exec_atr(struct atribuicao atr) {
    int i;
    struct valor valor_lido;
    // Le o rhs
    switch(atr.mod) {
        case ATRIB_VAL:
            valor_lido = atr.rhs_val;
            break;
        case ATRIB_VAR:
            // Busca variavel pelo nome
            for(i = 0; i < TAM_MEM; i = i + 1) {
                if (strcmp(atr.lhs.nom, mem[i].nom) == 0) {
                    valor_lido = mem[i].con;
                    break;
                }
            }
            if (i == TAM_MEM) {
                printf("Variavel nao encontrada\n");
                exit(2);
            }
            break;
    }

    // Escreve no lhs
    // Busca variavel pelo nome
    for(i = 0; i < TAM_MEM; i = i + 1) {
        if (strcmp(atr.lhs.nom, mem[i].nom) == 0) {
            break;
        }
    }
    if (i == TAM_MEM) {
        printf("Variavel nao encontrada\n");
        exit(2);
    }
    mem[i].con = valor_lido;

}

void executa(struct sentenca stn) {
    switch(stn.mod) {
        case STNC_DEC: exec_dec(stn.dec); break;
        case STNC_ATR: exec_atr(stn.atr); break;
    }
}

/************************************************
 * Caso de teste
 */
int main () {
    
    // Inicializa a memória
    int i;
    struct valor zero; 
    zero.mod = INT; 
    zero.i = 0;
    for(i = 0; i < TAM_MEM; i = i + 1) {
        mem[i].end = END_INI + i;
        mem[i].tip = LIVRE;
        strcpy(mem[i].nom," ");
        mem[i].con = zero;
    }

    /*
     * Constroi um programa
     */ 

    // sentenca:    char key
    struct acessa_variavel var1;
    var1.mod = VAR;
    var1.nom = "key";

    struct declaracao dec1;
    dec1.tip = CHAR;
    dec1.var = var1;

    struct sentenca st1;
    st1.mod = STNC_DEC;
    st1.dec = dec1;

    // sentenca:     key = 'k'
    struct valor val1;
    val1.mod = VAL_CHAR;
    val1.c = 'k';
 
    struct atribuicao atr1;
    atr1.mod = ATRIB_VAL;
    atr1.lhs = var1;
    atr1.rhs_val = val1;

    struct sentenca st2;
    st2.mod = STNC_ATR;
    st2.atr = atr1;


    // Mostra o programa

    // Executa o programa
    executa(st1);
    executa(st2);

    // Mostra a memória
    pprint_mem(END_INI, END_INI+10);

}