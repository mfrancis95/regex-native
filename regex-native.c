#include <stdio.h>
#include <stdlib.h>

typedef enum {
    CONCATENATION, EMPTY, KLEENE_PLUS, KLEENE_STAR, OPTIONAL, PRIMITIVE
} Type;

typedef struct Regex {
    struct Regex *arg1;
    struct Regex *arg2;    
    char character;
    Type type;
} Regex;

static Regex *newConcatenation(Regex *arg1, Regex *arg2) {
    Regex *concatenation = malloc(sizeof(Regex));
    concatenation->arg1 = arg1;
    concatenation->arg2 = arg2;
    concatenation->type = CONCATENATION;
    return concatenation;
}

static Regex *newEmpty(void) {
    Regex *empty = malloc(sizeof(Regex));
    empty->type = EMPTY;
    return empty;
}

static Regex *newKleenePlus(Regex *arg) {
    Regex *repetition = malloc(sizeof(Regex));
    repetition->arg1 = arg;
    repetition->type = KLEENE_PLUS;
    return repetition;
}

static Regex *newKleeneStar(Regex *arg) {
    Regex *repetition = malloc(sizeof(Regex));
    repetition->arg1 = arg;
    repetition->type = KLEENE_STAR;
    return repetition;
}

static Regex *newOptional(Regex *arg) {
    Regex *optional = malloc(sizeof(Regex));
    optional->arg1 = arg;
    optional->type = OPTIONAL;
    return optional;
}

static Regex *newPrimitive(char character) {
    Regex *primitive = malloc(sizeof(Regex));
    primitive->character = character;
    primitive->type = PRIMITIVE;
    return primitive;
}

static Regex *parseBase(int *index, char *string) {
    return newPrimitive(string[(*index)++]);
}

static Regex *parseFactor(int *index, char *string) {
    Regex *base = parseBase(index, string);
    switch (string[*index]) {
        case '?':
            (*index)++;
            return newOptional(base);
        case '*':
            (*index)++;
            return newKleeneStar(base);
        case '+':
            (*index)++;
            return newKleenePlus(base);
    }
    return base;
}

static Regex *parseTerm(int *index, char *string) {
    Regex *factor = newEmpty();
    while (string[*index]) {
        factor = newConcatenation(factor, parseFactor(index, string));
    }
    return factor;
}

static void printPrimitive(Regex *regex) {
    printf("if(argv[1][index++]!='%c')return 1;", regex->character);
}

static void printKleenePlus(Regex *regex) {
    switch (regex->arg1->type) {
        case PRIMITIVE:
            printPrimitive(regex->arg1);
            printf("while(argv[1][index++]=='%c');", regex->arg1->character);
    }
}

static void printKleeneStar(Regex *regex) {
    switch (regex->arg1->type) {
        case PRIMITIVE:
            printf("while(argv[1][index++]=='%c');", regex->arg1->character);       
    }
}

static void printOptional(Regex *regex) {
    switch (regex->arg1->type) {
        case PRIMITIVE:
            printf("if(argv[1][index]=='%c')index++;", regex->arg1->character);
    }
}

static void printConcatenation(Regex *regex) {
    switch (regex->arg1->type) {
        case CONCATENATION:
            printConcatenation(regex->arg1);
            break;
        case KLEENE_PLUS:
            printKleenePlus(regex->arg1);
            break;
        case KLEENE_STAR:
            printKleeneStar(regex->arg1);
            break;
        case OPTIONAL:
            printOptional(regex->arg1);
            break;
        case PRIMITIVE:
            printPrimitive(regex->arg1);
    }
    switch (regex->arg2->type) {
        case CONCATENATION:
            printConcatenation(regex->arg2);
            break;
        case KLEENE_PLUS:
            printKleenePlus(regex->arg2);
            break;
        case KLEENE_STAR:
            printKleeneStar(regex->arg2);
            break;
        case OPTIONAL:
            printOptional(regex->arg2);
            break;
        case PRIMITIVE:
            printPrimitive(regex->arg2);
    }
}

int main(int argc, char *argv[]) {
    int index = 0;
    Regex *regex = parseTerm(&index, argv[1]);
    printf("int main(int argc,char*argv[]){int index=0;");
    printConcatenation(regex);
    printf("return argv[1][index];}");
    return 0;
}