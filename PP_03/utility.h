#ifndef UTILITY_H
#define UTILITY_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_THREADS 3
#define SIZE_BUFFER 4096
#define TOK_DELIM " \t\n"

//Definição das estruturas

typedef struct input_list{
    int size_of;            /* tamanho da lista */
    int **list;             /* lista de inteiros */
};

void *calcPrimeNumbers(void *threadarg);

void *getMean(void *threadarg);

void *getMin(void *threadarg);

void *getMax(void *threadarg);

#endif //UTILITY_H
