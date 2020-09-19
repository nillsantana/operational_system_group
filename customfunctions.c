/*Header*/
#include "customfunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

long size_buffer = 4096;


void writeFile(int source_file, int destiny_file, char *buffer) {
    int n;
    while ((n = read(source_file, buffer, size_buffer)) > 0)
        if (write(destiny_file, buffer, n) != n) {
            printf("%s: Um problema ocorreu na escrita do arquivo %s\n", "nome programa", "arquivo 2");
            perror("causa");
            exit(3);
        }

    if (n == -1) {
        printf("%s: Um problema ocorreu na leitura do arquivo %s\n", "nome programa", "Arquivo 1");
        exit(2);
    }
}


