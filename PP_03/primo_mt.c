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
    int input;
    pthread_t threads[NUM_THREADS];

    printf("Informe um número: ");
    scanf("%d", &input);

    int r_t1 = pthread_create(&threads[0], NULL, calcPrimeNumbers, (void *) &input);
    if (r_t1)
        printf("ERROR; return code from pthread_create() is %d\n", r_t1);
    pthread_exit(NULL);
}