#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include <memory.h>

#define DEFAULT_MATRIX 1000

typedef struct {
    int rows;
    int cols;
    int **data;
} MatrixData;

typedef struct {
    int rank;
} T;

/* Variáveis globais */
int NUM_THREADS = 1;            /* Número de threads*/
T **thread;                    /* Estrutura global de T */

MatrixData *A;
MatrixData *B;
MatrixData *C;

/**
 *
 * @param rows
 * @param cols
 * @return
 */
MatrixData *makeMatrix(int rows, int cols) {
    MatrixData *matrix;
    matrix = malloc(sizeof(MatrixData) + 1);
    matrix->rows = rows;
    matrix->cols = cols;
    int **data = malloc(sizeof(long *) * (rows*cols));
    for (int x = 0; x < rows; x++) {
        data[x] = malloc(sizeof(double *) * cols + 1);
        for (int y = 0; y < cols; y++) {
            /* Gera os números aleatórios em um intervalo controlado (1 a 100) */
            int current = (1 +(rand() % 100));
//            data[x][y] = (int) malloc(sizeof(current) + 1);
            data[x][y] = current;
        }
    }
    matrix->data = data;
    return matrix;
}

/**
 *
 * @param rank
 * @return
 */
void *calcMatrix(void *rank) {
    int local_rank = *(int *) rank;         /* Variável local para ranking*/
    int rows = A->rows;
    int cols = A->cols;
    C->data = malloc(sizeof(double *) * (rows * cols) + 1);
//    C.data = calloc(rows * cols, sizeof(double *) * (rows * cols));

    int n = rows / NUM_THREADS;

    long long first_row = n * local_rank;     /* Ponto de partida da iteração */
    long long last_row = first_row + n;         /* Ponto máximo de iteração */

//    printf("thread[%d] first_i[%lld] last[%lld]\n", local_rank, first_row, last_row);

    for (long long row = first_row; row < last_row; row++) {
        C->data[row] = malloc(sizeof(double *) * cols + 1);
        for (long long col = 0; col < cols; col++) {
            C->data[row][col] = A->data[row][col] * B->data[row][col];
        }
    }
    pthread_exit((void *) rank);
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    time_t t_stop, t_start;
    int t_id = 0;                                           /* identificador das threads */
    int err_status;                                         /* variável que recebe o status do erro */
    int rows = DEFAULT_MATRIX, cols = DEFAULT_MATRIX;
    if (argc > 3) {
        NUM_THREADS = strtol(argv[1], &argv[1], 10);
        rows = strtol(argv[2], &argv[2], 10);
        cols = strtol(argv[3], &argv[3], 10);
    }
    MatrixData matrix_tmp = {.cols=cols, .rows=rows, .data=malloc(sizeof(MatrixData *) + 1)};
    pthread_t thread_list[NUM_THREADS];                     /* Define uma lista de threads */
    thread = malloc(sizeof(T *) * NUM_THREADS + 1);    /* Realiza a alocação dinâmica das threads T[0, ..., n] */
    pthread_attr_t attr;
    void *status;

    t_start = clock();

    A = makeMatrix(rows, cols);
    B = makeMatrix(rows, cols);
    C = malloc(sizeof(MatrixData) + 1);

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    while (t_id < NUM_THREADS) {
        thread[t_id] = malloc(sizeof(T *) * NUM_THREADS + 1);   /* Faz a alocação dinâmica de uma thread T[i]*/
        thread[t_id]->rank = t_id;                                   /* Atribui o identificador à thread */
        /* Verifica se houve algum erro na criação da thread T[i]*/
        err_status = pthread_create(&(thread_list[t_id]), &attr, calcMatrix, (void *) &thread[t_id]->rank);
        if (err_status != 0) {
            printf("Não foi possível criar a thread [%d]: %d\n", t_id, err_status);
            exit(EXIT_FAILURE);
        }
        t_id++;
    }
//    printf("Juntando\n");

    /* Coloca as threads em "modo de espera" */
    t_id = 0;
    pthread_attr_destroy(&attr);
    while (t_id < NUM_THREADS) {
        err_status = pthread_join(thread_list[t_id], &status);
        if (err_status) {
            printf("Não foi possível juntar a thread [%d]: %d\n", t_id, err_status);
//            exit(EXIT_FAILURE);
        }
        t_id++;
    }
    free(A);
    free(B);
    free(C);

    t_stop  = clock();

    long final_time = ((double) t_stop - t_start) / (CLOCKS_PER_SEC / 1000);

    printf("%d, %ld\n", NUM_THREADS, final_time);

    exit(0);
}