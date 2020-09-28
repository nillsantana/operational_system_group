/* Universidade Federal da Bahia
 * Sistemas Operacionais (MAT566). Semestre Letivo Suplementar 2020
 * Componentes: Elivelton Rangel, Jane Barbosa, Mariese Conceição, Nilvan Santana
 * Exercício prático parte 1: Cópia de arquivo simples
 * Programa que cria uma cópia exata de um arquivo para outro arquivo
 */
#include "customfunctions.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Declaração das variáveis globais */
extern char *PROGRAM_NAME;
extern long SIZE_BUFFER; //Tamanho do buffer é de 4kb
extern FileDirManagement fd_management;

int main(int argc, char **argv) {
    SIZE_BUFFER = 4096;
    PROGRAM_NAME = argv[0]; // nome do programa recebido como primeiro parâmetro (argv[0])
    fd_management.source = argv[1]; // nome do arquivo de origem recebido como segundo parâmetro (argv[1])
    fd_management.destination = argv[2]; // nome do arquivo de origem recebido como segundo parâmetro (argv[2])
    int file_source, file_destination; // Variáveis que receberão os arquivos que serão utilizados
    char *buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));

    /* Verificando entrada de argumentos */
    if (argc != 3) {
        print_errors(INVALID_PARAMS);
        exit(EXIT_FAILURE);
    }

    // Verifica o arquivo de origem para tratar os possíveis erros de leitura
    file_source = checkFileRead(file_source, fd_management.source);
    // Verifica o arquivo de destino para tratar os possíveis erros de leitura/escrita
    file_destination = checkFileWrite(file_destination, fd_management.destination);

    /* Se não tiver nenhum problema nas etapas anteriores, então o conteúdo do arquivo de entrada é escrito
     * no arquivo de saída */
    writeFile(file_source, file_destination, buffer);

    /* conta o número de bites utilizando a função countFileSize */
    int size_bytes_file = countFileSize(fd_management.source, buffer);

    /* Apresenta uma resposta amigável caso as operações anteriores executem sem erros (type SUCCESS)*/
    printf("%s: foram copiados %d bytes do arquivo %s para o arquivo %s\n", PROGRAM_NAME, size_bytes_file,
           fd_management.source, fd_management.destination);

    /* Fecha os arquivos de origem e destino */
    close(file_source);
    close(file_destination);
    exit(0);
}
