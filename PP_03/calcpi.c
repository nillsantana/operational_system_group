#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>//time

#define LIMIT_DEPTH 1000000000

/* Estrutura de armazenamento dos dados da thread */
typedef struct {
    int rank;                   /* rank ou identificador da thread */
    double sum;                 /* somatório da thread */
} T;

/* variáveis globais */
int NUM_THREADS = 1;            /* Número de threads (mínimo 1). É alterado caso haja algum argumento (argv) */
T **threadI;                     /* Estrutura global de T */
int count = 0;

/**
 * @brief Função que calcula um valor parcial de PI e armazena em uma estrutura global T (threadI)
 * @param rank Identificador da Thread
 * @return Sem retorno
 */
void *calc_pi(void *rank) {
    int local_rank = *(int *) rank;         /* Variável local para ranking*/
    int n = LIMIT_DEPTH / NUM_THREADS;      /* Número máximo de iterações por thread */
    double signal = 1;                      /* fator de operação (-1)^n */
    double local_sum;                       /* Variável local para o somatório */
    long long i;                            /* Variável de iteração */
    long long first_i = n * local_rank;     /* Ponto de partida da iteração */
    long long last_i = first_i + n;         /* Ponto máximo de iteração */

    if (first_i % 2)                        /* Verifica se o ponto de partida é impar (result != 0), para mudar o fator de operação (-n)) */
        signal = -1;

//    printf("thread[%d] first_i[%lld] last[%lld]\n", local_rank, first_i, last_i);

    for (i = first_i; i < last_i; i++) {
        local_sum += signal / (2 * i + 1);  /* Aplica a fórmula de Leibniz */
        signal = -signal;                   /* Muda o sinal (+ ou -) para a próxima iteração */
    }

    /* método alternativo utilizando a biblioteca math (mais custoso */
//    for (i = first_i; i < last_i; i++) {
//        local_sum += pow(-1, i) / (2 * i + 1);  /* Aplica a fórmula de Leibniz */
//    }
    threadI[local_rank]->sum = local_sum;    /* Adiciona o valor do somatório em uma estrutura global de um threat T[i] */
    pthread_exit((void *) rank);

}

//(fim*x+ms) - (inicio*x+ms) = tempo
#define GET_MS(ini, fim)  ((fim.tv_sec * 1000000 + fim.tv_usec) \
			- (ini.tv_sec * 1000000 + ini.tv_usec))

/**
 * @brief Função principal do calcpi
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    struct timeval t_stop, t_start;
    int t_id = 0;                                           /* identificador das threads */
    int err_status;                                         /* variável que recebe o status do erro */
    double pi = 0;
    /* recebe como parâmetro o número de threads */
    if (argc > 1) {
        NUM_THREADS = strtol(argv[1], &argv[1], 10);
    }

    pthread_t thread_list[NUM_THREADS];                     /* Define uma lista de threads */
    threadI = malloc(sizeof(T *) * NUM_THREADS + 1);    /* Realiza a alocação dinâmica das threads T[0, ..., n] */
    gettimeofday (&t_start, NULL );

    /* Inicializa as threads */
    while (t_id < NUM_THREADS) {
        threadI[t_id] = malloc(sizeof(T *) * NUM_THREADS + 1);   /* Faz a alocação dinâmica de uma thread T[i]*/
        threadI[t_id]->rank = t_id;                                   /* Atribui o identificador à thread */
        /* Verifica se houve algum erro na criação da thread T[i]*/
        err_status = pthread_create(&(thread_list[t_id]), NULL, calc_pi, (void *) &threadI[t_id]->rank);
        if (err_status != 0) {
            printf("Não foi possível criar a thread [%d]: %d\n", t_id, err_status);
            exit(EXIT_FAILURE);
        }
        t_id++;
    }

    /* Coloca as threads em "modo de espera" */
    t_id = 0;
    while (t_id < NUM_THREADS) {
        pthread_join(thread_list[t_id], NULL);
        t_id++;
    }

    /* Quando as operações forem concluídas, realiza o somatório das operações (sum)*/
    t_id = 0;
    while (t_id < NUM_THREADS) {
        pi += threadI[t_id]->sum;
        t_id++;
    }
    pi *= 4.0;                  /* Atribui o valor real de PI (já que a fórmula de Leibniz = pi/4) */

    gettimeofday (&t_stop, NULL );

//    printf("Número de threads: %d\n", NUM_THREADS);
//    printf("Resultado: %.16f\n", pi);
//    printf("Tempo de execucao (ms): %ld\n", GET_MS(t_start, t_stop)); //Tempo em milisegundos

    printf("%d, %.12f, %ld\n", NUM_THREADS, pi, (t_stop.tv_sec * 1000 - t_start.tv_sec * 1000));
    return 0;
}