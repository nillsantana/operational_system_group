#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

extern int errno;

long size_buffer = 4096; //Tamanho do buffer é de 4kb
char *program_name;

enum result_types_ref {
    INVALID_PARAMS,
    SUCCESS,
} type;

void print_result(enum result_types_ref typesRef, char **argv, char *buffer) {
    buffer = (char *) calloc(size_buffer, sizeof(char));
    int frd, sz;

    switch (typesRef) {
        case INVALID_PARAMS:
            perror("Parâmetros inválidos!\n"
                   "Informe os nomes dos arquivos de origem e destino.\n"
                   "Ex: ./filecopy <arquivo_origem.txt> <arquivo_destino.txt>\n"
                   "Obs.: Se o arquivo conter espaço, utilize aspas duplas para selecioná-lo.\n"
                   "Ex.: ./filecopy <\"Arquivo de origem.txt\"> <\"Arquivo de destino.txt\">\n");
            break;

        case SUCCESS:
            frd = open(argv[2], O_RDONLY);
            sz = read(frd, buffer, size_buffer);
            printf("%s: foram copiados %d bytes do arquivo %s para o arquivo %s\n", program_name, sz, argv[1], argv[2]);
            break;
    }
}

int checkFileRead(int file, char *source) {
    if ((file = open(source, O_RDONLY)) == -1) {
        printf("Erro: %d\n", errno);
        printf("%s: não foi possível abrir o arquivo %s\n", program_name, source);
        perror("causa");
        exit(EXIT_FAILURE);
    }
    return file;
}

int checkFileWrite(int file, char *source) {
    if ((file = open(source, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == 1) {
        printf("Erro: %d\n", errno);
        printf("%s: não foi possível abrir o arquivo %s\n", program_name, source);
        perror("causa");
        exit(EXIT_FAILURE);
    }
    return file;
}


void writeFile(int source_file, int destiny_file, char *buffer) {
    int n;
    while ((n = read(source_file, buffer, size_buffer)) > 0)
        if (write(destiny_file, buffer, n) != n) {
            printf("%s: Ocorreu um problema na escrita do arquivo %s\n", program_name, "arquivo 2");
            perror("causa");
            exit(EXIT_FAILURE);
        }

    if (n == -1) {
        printf("%s: Ocorreu um problema na leitura do arquivo %s\n", program_name, "Arquivo 1");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    program_name = argv[0];
    int file[2];
    char *buffer = (char *) calloc(size_buffer, sizeof(char));
    int n;
    ssize_t bytes_written;

    /* Verificando entrada de argumentos (não pode ser diferente de 2 - arquivo_origem arquivo_destino)*/
    if (argc != 3) {
        print_result(INVALID_PARAMS, argv, buffer);
        exit(EXIT_FAILURE);
    }

    // Verifica o arquivo de origem para tratar os possíveis erros de leitura
    file[0] = checkFileRead(file[0], argv[1]);
    // Verifica o arquivo de destino para tratar os possíveis erros de leitura\escrita
    file[1] = checkFileWrite(file[1], argv[2]);

    /* Se não tiver nenhum problema com os arquivos anteriores, então o conteúdo do arquivo de entrada é escrito
     * no arquivo de saída */
    writeFile(file[0], file[1], buffer);

    /* Apresenta uma resposta amigável caso a operação anterior execute sem erros */
    print_result(SUCCESS, argv, buffer);
    /* Fecha os arquivos */
    close(file[0]);
    close(file[1]);
    exit(0);
}