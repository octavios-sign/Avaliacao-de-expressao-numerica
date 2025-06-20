#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "calculadora.h"

#define MAX 100
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------------------------- Estruturas de Pilha --------------------------- //
typedef struct {
    char items[MAX][256];
    int top;
} StackStr;

typedef struct {
    float items[MAX];
    int top;
} StackFloat;

// -------------------------- Funções de Pilha --------------------------- //
void initStr(StackStr* s) { s->top = -1; }
int emptyStr(StackStr* s) { return s->top == -1; }
void pushStr(StackStr* s, char* val) {
    if (s->top < MAX - 1) {
        strcpy(s->items[++s->top], val);
    }
}
char* popStr(StackStr* s) {
    if (!emptyStr(s)) {
        return s->items[s->top--];
    }
    return "";
}
char* peekStr(StackStr* s) { return s->items[s->top]; }

void initFloat(StackFloat* s) { s->top = -1; }
void pushFloat(StackFloat* s, float val) { s->items[++s->top] = val; }
float popFloat(StackFloat* s) { return s->items[s->top--]; }
int sizeFloat(StackFloat* s) { return s->top + 1; }

// -------------------------- Funções Auxiliares --------------------------- //
int isOperator(char* token) {
    return (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || strcmp(token, "*") == 0 ||
            strcmp(token, "/") == 0 || strcmp(token, "%") == 0 || strcmp(token, "^") == 0);
}

int isFunction(char* token) {
    return (strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 || strcmp(token, "tg") == 0 ||
            strcmp(token, "log") == 0 || strcmp(token, "raiz") == 0);
}

int precedence(char* op) {
    if (strcmp(op, "^") == 0) return 4;
    if (isFunction(op)) return 4;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 3;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 2;
    return 0;
}

int isLeftAssociative(char* op) {
    return strcmp(op, "^") != 0;
}

int isNumeric(char* s) {
    if (!s || s[0] == '\0') return 0;
    char *end;
    strtod(s, &end);
    return *end == '\0';
}

int tokenize(char* expr, char tokens[][20]) {
    int i = 0, j = 0, k = 0;
    while (expr[i]) {
        if (isspace(expr[i])) { i++; continue; }
        if (isdigit(expr[i]) || (expr[i] == '.' && isdigit(expr[i+1])) || ((expr[i] == '-' || expr[i] == '+') && isdigit(expr[i+1]) && (i == 0 || strchr("([{^*/+-%", expr[i-1])))) {
            j = 0;
            if (expr[i] == '-' || expr[i] == '+') tokens[k][j++] = expr[i++];
            while (isdigit(expr[i]) || expr[i] == '.') tokens[k][j++] = expr[i++];
            tokens[k++][j] = '\0';
        } else if (isalpha(expr[i])) {
            j = 0;
            while (isalpha(expr[i])) tokens[k][j++] = expr[i++];
            tokens[k++][j] = '\0';
        } else {
            tokens[k][0] = expr[i++];
            tokens[k++][1] = '\0';
        }
    }
    return k;
}

// --- FUNÇÃO DE VALIDAÇÃO (Restaurada para detecção robusta) ---
int validatePostfix(char tokens[][20], int n) {
    StackFloat stack;
    initFloat(&stack);

    if (n == 0) return 0;

    for (int i = 0; i < n; i++) {
        if (isNumeric(tokens[i])) {
            pushFloat(&stack, 1.0); // Empilha um valor qualquer
        } else if (isOperator(tokens[i])) {
            if (sizeFloat(&stack) < 2) return 0; // Inválido
            popFloat(&stack);
            popFloat(&stack);
            pushFloat(&stack, 1.0);
        } else if (isFunction(tokens[i])) {
            if (sizeFloat(&stack) < 1) return 0; // Inválido
            popFloat(&stack);
            pushFloat(&stack, 1.0);
        } else if (strcmp(tokens[i], "(") == 0 || strcmp(tokens[i], ")") == 0) {
            return 0; // Expressões pós-fixas não têm parênteses
        } else {
            return 0; // Token inválido
        }
    }
    // Se no final sobrar exatamente um elemento na pilha, a expressão é válida
    return (sizeFloat(&stack) == 1);
}

// --- FUNÇÃO DE DETECÇÃO (Para ser chamada pelo main.c) ---
// Retorna 1 se for Pós-fixa, 0 se for Infixa
int detectarNotacao(char *expr) {
    char tokens[100][20];
    int n = tokenize(expr, tokens);

    if (validatePostfix(tokens, n)) {
        return 1;
    }
    return 0;
}


// --- FUNÇÕES DE CONVERSÃO E CÁLCULO ---

char *getFormaInFixa(char *Str) {
    static char resultado[512];
    char tokens[100][20];
    int n = tokenize(Str, tokens);
    StackStr s;
    initStr(&s);
    for (int i = 0; i < n; i++) {
        if (isNumeric(tokens[i])) {
            pushStr(&s, tokens[i]);
        } else if (isOperator(tokens[i])) {
            if (s.top < 1) return "ERRO: Faltam operandos";
            char b[128], a[128];
            strcpy(b, popStr(&s));
            strcpy(a, popStr(&s));
            char temp[256];
            sprintf(temp, "(%s %s %s)", a, tokens[i], b);
            pushStr(&s, temp);
        } else if (isFunction(tokens[i])) {
            if (s.top < 0) return "ERRO: Faltam operandos para funcao";
            char a[128];
            strcpy(a, popStr(&s));
            char temp[256];
            sprintf(temp, "%s(%s)", tokens[i], a);
            pushStr(&s, temp);
        }
    }
    if (s.top != 0) return "ERRO: Expressao malformada";
    strcpy(resultado, popStr(&s));
    if (resultado[0] == '(' && resultado[strlen(resultado) - 1] == ')') {
        resultado[strlen(resultado) - 1] = '\0';
        strcpy(resultado, resultado + 1);
    }
    return resultado;
}

char *getFormaPosFixa(char *Str) {
    static char resultado[512];
    char tokens[100][20], output[100][20];
    int n = tokenize(Str, tokens), outSize = 0;
    StackStr opStack;
    initStr(&opStack);
    for (int i = 0; i < n; i++) {
        if (isNumeric(tokens[i])) strcpy(output[outSize++], tokens[i]);
        else if (isFunction(tokens[i])) pushStr(&opStack, tokens[i]);
        else if (isOperator(tokens[i])) {
            while (!emptyStr(&opStack) && strcmp(peekStr(&opStack), "(") != 0 &&
                   (precedence(peekStr(&opStack)) > precedence(tokens[i]) ||
                    (precedence(peekStr(&opStack)) == precedence(tokens[i]) && isLeftAssociative(tokens[i])))) {
                strcpy(output[outSize++], popStr(&opStack));
            }
            pushStr(&opStack, tokens[i]);
        } else if (strcmp(tokens[i], "(") == 0) pushStr(&opStack, tokens[i]);
        else if (strcmp(tokens[i], ")") == 0) {
            while (!emptyStr(&opStack) && strcmp(peekStr(&opStack), "(") != 0) strcpy(output[outSize++], popStr(&opStack));
            if (!emptyStr(&opStack)) popStr(&opStack);
            if (!emptyStr(&opStack) && isFunction(peekStr(&opStack))) strcpy(output[outSize++], popStr(&opStack));
        }
    }
    while (!emptyStr(&opStack)) strcpy(output[outSize++], popStr(&opStack));
    resultado[0] = '\0';
    for (int i = 0; i < outSize; i++) {
        strcat(resultado, output[i]);
        if (i < outSize - 1) strcat(resultado, " ");
    }
    return resultado;
}

float getValorPosFixa(char *StrPosFixa) {
    char tokens[100][20];
    int n = tokenize(StrPosFixa, tokens);
    StackFloat s;
    initFloat(&s);
    for (int i = 0; i < n; i++) {
        if (isNumeric(tokens[i])) pushFloat(&s, atof(tokens[i]));
        else if (isOperator(tokens[i])) {
            if (sizeFloat(&s) < 2) return NAN;
            float b = popFloat(&s), a = popFloat(&s), r = 0.0;
            if (strcmp(tokens[i], "+") == 0) r = a + b;
            else if (strcmp(tokens[i], "-") == 0) r = a - b;
            else if (strcmp(tokens[i], "*") == 0) r = a * b;
            else if (strcmp(tokens[i], "/") == 0) { if (b == 0) return NAN; r = a / b; }
            else if (strcmp(tokens[i], "%") == 0) { if (b == 0) return NAN; r = fmod(a, b); }
            else if (strcmp(tokens[i], "^") == 0) r = pow(a, b);
            pushFloat(&s, r);
        } else if (isFunction(tokens[i])) {
            if (sizeFloat(&s) < 1) return NAN;
            float a = popFloat(&s), r = 0.0;
            if (strcmp(tokens[i], "sen") == 0) r = sin(a * M_PI / 180.0);
            else if (strcmp(tokens[i], "cos") == 0) r = cos(a * M_PI / 180.0);
            else if (strcmp(tokens[i], "tg") == 0) r = tan(a * M_PI / 180.0);
            else if (strcmp(tokens[i], "log") == 0) { if (a <= 0) return NAN; r = log10(a); }
            else if (strcmp(tokens[i], "raiz") == 0) { if (a < 0) return NAN; r = sqrt(a); }
            pushFloat(&s, r);
        }
    }
    if (sizeFloat(&s) != 1) return NAN;
    return popFloat(&s);
}

float getValorInFixa(char *StrInFixa) {
    char *posfixa = getFormaPosFixa(StrInFixa);
    if(strstr(posfixa, "ERRO") != NULL) return NAN;
    return getValorPosFixa(posfixa);
}