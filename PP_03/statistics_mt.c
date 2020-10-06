#include "utility.c"

#define MAX_INPUT 200

//Global declarations
int MIN, MAX, AVERAGE;

/**
 * @brief Método principal
 * @param argc
 * @param argv
 * @return Exit or Fail
 */
int main(int argc, char **argv) {
    char input[MAX_INPUT] = "";
    char *input_result;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;
    int rstatus;

    struct input_list lst = {.size_of=0, .list=malloc(MAX_INPUT * sizeof(int *) + 1)};
    printf("Informe a sequência numérica: \n");
    input_result = fgets(input, sizeof(input), stdin);
    fflush(stdout);
    input_split(input, &lst);

    if (input_result == NULL)
        return 0;

    /* Inicializa e configura a variável de atributo das threads */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int r_t1 = pthread_create(&threads[0], &attr, getMin, (void *) &lst);
    int r_t2 = pthread_create(&threads[1], &attr, getMax, (void *) &lst);
    int r_t3 = pthread_create(&threads[2], &attr, getMean, (void *) &lst);

    if (r_t1)
        printf("ERROR; o código retornado de pthread_create() é %d\n", r_t1);
    if (r_t2)
        printf("ERROR; o código retornado de pthread_create() é %d\n", r_t2);
    if (r_t3)
        printf("ERROR; o código retornado de pthread_create() é %d\n", r_t3);

    /* Libera o atributo da thread e aguarda pela finalização de outras threads */
    pthread_attr_destroy(&attr);
    for (int t = 0; t < NUM_THREADS; t++){
        rstatus = pthread_join(threads[t], &status);
        if(rstatus) {
            printf("ERROR[%d]; o código retornado de pthread_join() é %d\n", t, rstatus);
        }
    }

    printf("O valor médio é %d\n", AVERAGE);
    printf("O valor máximo é %d\n", MAX);
    printf("O valor mínimo é %d\n", MIN);
    pthread_exit(NULL);
}