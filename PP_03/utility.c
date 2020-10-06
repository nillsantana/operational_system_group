#include "utility.h"

extern int MIN, MAX, AVERAGE;

void *getMean(void *threadarg){
    int soma = 0;
    int current;
    struct input_list *lst;
    lst = (struct input_list *) threadarg;
    for(int i = 0; i < lst->size_of; i++){
        current = lst->list[i];
        soma += current;
    }
    AVERAGE = soma/lst->size_of;
    pthread_exit((void*) threadarg);
}

void *getMin(void *threadarg) {
    struct input_list *lst;
    lst = (struct input_list *) threadarg;
    int min = lst->list[0];
    for(int i = 0; i < lst->size_of; i++){
        if (lst->list[i] < min)
            min = lst->list[i];
    }
    MIN = min;
    pthread_exit((void*) threadarg);
}

void *getMax(void *threadarg) {
    struct input_list *lst;
    lst = (struct input_list *) threadarg;
    int max = lst->list[0];
    for(int i = 0; i < lst->size_of; i++){
        if (lst->list[i] > max)
            max = lst->list[i];
    }
    MAX = max;
    pthread_exit((void*) threadarg);
}

int isPrimeNumber(int number){
    int prime = 1;
    for(int i = 2; i<= number / 2; i++){
        if(number % i == 0)
            prime = 0;
    }
    return prime;
}

void *calcPrimeNumbers(void *threadarg) {
    int *n_max;
    int i, primes = 0;
    n_max = (int *) threadarg;

    for (i = 2; i <= *n_max; i++) {
        if(isPrimeNumber(i)){
            printf("%d ", i);
            primes++;
        }
    }
    pthread_exit((void*) threadarg);
}



#define TOK_BUFSIZE 64

/**
   @brief traçar uma linha em tokens (muito ingenuamente).
   @param line linha de entrada.
   @return Número de argumentos de @line.
 */
int input_split(char *line, struct input_list *list) {
    int i = 0;
    int **items = malloc(SIZE_BUFFER * sizeof(char *) + 1);
    char *input_line;

    input_line = strtok(line, TOK_DELIM);
    while (input_line != NULL) {
        items[i] = malloc(TOK_BUFSIZE * sizeof(input_line) + 1);
        items[i] = strtol(input_line, &input_line, 10);
        i++;
        input_line = strtok(NULL, TOK_DELIM);
    }
    items[i] = NULL;
    list->size_of = i;
    list->list = items;
    return i;
}
