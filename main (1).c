#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "calculadora.h"

// Protótipos das funções que estão em calculadora.c
int isOperator(char* token);
int isFunction(char* token);
int isNumeric(char* s);
int tokenize(char* expr, char tokens[][20]);
int detectarNotacao(char *expr);

// ADICIONADO: Protótipo para a função de explicação
void calcularComExplicacao(char* posFixaStr);


// Função para imprimir o estado atual da pilha de floats
void imprimirPilha(float* stack, int top) {
    printf("     Pilha atual: [ ");
    for (int i = 0; i <= top; i++) {
        printf("%.2f ", stack[i]);
    }
    printf("]\n");
}

// Função que gerencia todo o processo para uma expressão
void processarEExplicar(char* expressao) {
    // Pré-processamento: remove vírgulas e converte para minúsculas
    for (int i = 0; expressao[i]; i++) {
        if (expressao[i] == ',') expressao[i] = '.';
        expressao[i] = tolower(expressao[i]);
    }

    // Chama a função robusta de detecção
    int isPosfixa = detectarNotacao(expressao);
    
    printf("\n================================================================\n");
    printf("               ANALISE DA EXPRESSAO: %s\n", expressao);
    printf("================================================================\n");

    char *convertida;
    char *paraCalcular;

    printf("\n--- ETAPA 1: CONVERSAO DE NOTACAO ---\n");
    if (isPosfixa) {
        printf("Expressao identificada como: POS-FIXA.\n");
        printf("Convertendo para a forma Infixa para visualizacao.\n");
        printf("  - Original (Pos-fixa): %s\n", expressao);
        convertida = getFormaInFixa(expressao);
        printf("  - Convertida (Infixa): %s\n", convertida);
        paraCalcular = expressao;
    } else {
        printf("Expressao identificada como: INFIXA.\n");
        printf("Convertendo para a forma Pos-fixa para o calculo.\n");
        printf("  - Original (Infixa):   %s\n", expressao);
        convertida = getFormaPosFixa(expressao);
        printf("  - Convertida (Pos-fixa): %s\n", convertida);
        paraCalcular = convertida;
    }

    if(strstr(convertida, "ERRO") != NULL) {
        printf("\nERRO na conversao. Verifique a expressao.\n");
        return;
    }

    calcularComExplicacao(paraCalcular);
}


// --- FUNÇÃO DE CÁLCULO COM EXPLICAÇÃO ---
void calcularComExplicacao(char* posFixaStr) {
    printf("\n--- ETAPA 2: CALCULO DO RESULTADO (a partir da forma Pos-fixa) ---\n");
    printf("O calculo e feito lendo cada item (token) da expressao pos-fixa.\nNumeros sao empilhados. Operadores desempilham valores, calculam e empilham o resultado.\n");
    
    char tokens[100][20];
    int n = tokenize(posFixaStr, tokens);
    
    float stackItems[100];
    int top = -1;
    
    for (int i = 0; i < n; i++) {
        printf("\nLendo token: \"%s\"\n", tokens[i]);
        if (isNumeric(tokens[i])) {
            float val = atof(tokens[i]);
            stackItems[++top] = val;
            printf("  -> E um numero. Empilha %g.\n", val);
        } else if (isOperator(tokens[i])) {
            printf("  -> E um operador. Requer 2 operandos.\n");
            if (top < 1) { printf("ERRO: Faltam operandos na pilha!\n"); return; }
            float b = stackItems[top--];
            float a = stackItems[top--];
            float r = 0.0;
            printf("     - Desempilha %.2f (operando 'b') e %.2f (operando 'a').\n", b, a);
            
            if (strcmp(tokens[i], "+") == 0) { r = a + b; printf("     - Calcula: %.2f + %.2f = %.2f\n", a, b, r); }
            else if (strcmp(tokens[i], "-") == 0) { r = a - b; printf("     - Calcula: %.2f - %.2f = %.2f\n", a, b, r); }
            else if (strcmp(tokens[i], "*") == 0) { r = a * b; printf("     - Calcula: %.2f * %.2f = %.2f\n", a, b, r); }
            else if (strcmp(tokens[i], "/") == 0) { r = a / b; printf("     - Calcula: %.2f / %.2f = %.2f\n", a, b, r); }
            else if (strcmp(tokens[i], "%") == 0) { r = fmod(a,b); printf("     - Calcula: %.2f %% %.2f = %.2f\n", a, b, r); }
            else if (strcmp(tokens[i], "^") == 0) { r = pow(a,b); printf("     - Calcula: %.2f ^ %.2f = %.2f\n", a, b, r); }

            stackItems[++top] = r;
            printf("     - Empilha o resultado %.2f.\n", r);
        } else if (isFunction(tokens[i])) {
            printf("  -> E uma funcao. Requer 1 operando.\n");
            if (top < 0) { printf("ERRO: Falta operando na pilha!\n"); return; }
            float a = stackItems[top--];
            float r = 0.0;
            printf("     - Desempilha %.2f (operando 'a').\n", a);

            if (strcmp(tokens[i], "sen") == 0) { r = sin(a * M_PI/180.0); printf("     - Calcula: sen(%.2f) = %f\n", a, r); }
            else if (strcmp(tokens[i], "cos") == 0) { r = cos(a * M_PI/180.0); printf("     - Calcula: cos(%.2f) = %f\n", a, r); }
            else if (strcmp(tokens[i], "tg") == 0) { r = tan(a * M_PI/180.0); printf("     - Calcula: tg(%.2f) = %f\n", a, r); }
            else if (strcmp(tokens[i], "log") == 0) { r = log10(a); printf("     - Calcula: log(%.2f) = %f\n", a, r); }
            else if (strcmp(tokens[i], "raiz") == 0) { r = sqrt(a); printf("     - Calcula: raiz(%.2f) = %f\n", a, r); }

            stackItems[++top] = r;
            printf("     - Empilha o resultado %f.\n", r);
        }
        imprimirPilha(stackItems, top);
    }
    printf("\n----------------------------------------------------------------\n");
    if(top == 0) {
        printf("Fim da expressao. O resultado final e o ultimo valor na pilha.\n");
        printf(">>> RESULTADO FINAL: %f\n", stackItems[top]);
    } else {
        printf("ERRO: A expressao e invalida. Sobraram %d elementos na pilha.\n", top + 1);
    }
}

void mostrarMenu() {
    printf("\n=== AVALIADOR DE EXPRESSOES MATEMATICAS ===\n");
    printf("1. Converter e calcular expressao\n");
    printf("2. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    char expressao[512];
    int opcao;
    
    do {
        mostrarMenu();
        if (scanf("%d", &opcao) != 1) { 
             while(getchar() != '\n');
             opcao = 0; 
        }
        while (getchar() != '\n');
        
        switch (opcao) {
            case 1: {
                printf("\nDigite a expressao (ex: (3+4)*5 ou 3 4 + 5 *): ");
                fgets(expressao, sizeof(expressao), stdin);
                expressao[strcspn(expressao, "\n")] = 0;
                
                if (strlen(expressao) == 0) {
                    printf("Expressao vazia!\n");
                    break;
                }
                processarEExplicar(expressao);
                break;
            }
            case 2:
                printf("\nSaindo do programa...\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }
    } while (opcao != 2);
    
    return 0;
}