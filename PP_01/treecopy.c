/* Universidade Federal da Bahia
 * Sistemas Operacionais (MAT566). Semestre Letivo Suplementar 2020
 * Componentes: Elivelton Rangel, Jane Barbosa, Mariese Conceição, Nilvan Santana
 * Exercício prático parte 2: Cópia recursiva de uma árvore de diretórios
 * Programa que cria uma cópia exata de um diretório DiretorioOrigem no novo diretório DiretorioDestino.
 */
#include "customfunctions.c"
#include <stdio.h>
#include <stdlib.h>

/* Declaração das variáveis globais */
extern char *PROGRAM_NAME;
extern long SIZE_BUFFER; //Tamanho do buffer é de 4kb
extern FileDirManagement fd_management;
long COUNT_DIR, COUNT_FILES, COUNT_BYTES_SIZE;

int main(int argc, char **argv) {
    SIZE_BUFFER = 4096;
    PROGRAM_NAME = argv[0]; // nome do programa recebido como primeiro parâmetro (argv[0])
    fd_management.source = argv[1]; // nome do diretório de origem recebido como segundo parâmetro (argv[1])
    fd_management.destination = argv[2]; // nome do diretório de origem recebido como segundo parâmetro (argv[2])

    //    Verificando entrada de argumentos (não pode ser diferente de 2 - arquivo_origem arquivo_destino)
    if (argc != 3) {
        print_errors(INVALID_PARAM_DIR);
        exit(EXIT_FAILURE);
    }
    /* Executa a verificação dos diretórios */
    checkDir(fd_management.source, fd_management.destination);
    /* Se o processo anterior executar com sucesso, uma mensagem amigável é exibida com o reultado da operação */
    printf("\n\n%s: Foram copiados %ld diretórios, %ld arquivos e %ld bytes de %s para %s\n", PROGRAM_NAME,
           COUNT_DIR, COUNT_FILES, COUNT_BYTES_SIZE, fd_management.source, fd_management.destination);
    exit(0);
}
