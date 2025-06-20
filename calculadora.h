#ifndef CALCULADORA_H
#define CALCULADORA_H

typedef struct {
    char posFixa[512]; // Expressão na forma pos-fixa, como 3 12 4 + *
    char inFixa[512]; // Expressão na forma infixa, como 3 * (12 + 4)
    float Valor; // Valor numérico da expressão
} Expressao;

// Retorna a forma inFixa de uma string posFixa
char *getFormaInFixa(char *Str);

// Retorna a forma posFixa de uma string inFixa
char *getFormaPosFixa(char *Str);

// Calcula o valor de uma string na forma posFixa
float getValorPosFixa(char *StrPosFixa);

// Calcula o valor de uma string na forma inFixa
float getValorInFixa(char *StrInFixa);

#endif