/* Universidade Federal da Bahia
 * Sistemas Operacionais (MATA58). Semestre Letivo Suplementar 2020
 * Componentes: Elivelton Rangel, Jane (sobrenome), Mariese (sobrenome), Nilvan Santana
 * Exercício prático parte 1: Cópia de arquivo simples
 * Programa que cria uma cópia exata de um arquivo para outro arquivo
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

extern int errno;

long SIZE_BUFFER = 4096; //Tamanho do buffer é de 4kb
char *PROGRAM_NAME;

enum result_types_ref {
    INVALID_PARAMS,
    OPEN_ERROR,
    WRITE_ERROR,
    READ_ERROR,
    SUCCESS_FILE,
} type;

//Estrutura para armazenar os arquivos de origem e destino passados por parâmetro (linha de código)
typedef struct file_dir_management {
    char *source;
    char *destination;
} FileDirManagement;

FileDirManagement file_names;

//Função que exibe as mensagens amigáveis caso um erro ocorra ou quando o processo é realizado com sucesso.
void print_errors(enum result_types_ref typesRef) {

    switch (typesRef) {
        case INVALID_PARAMS:
            perror("Parâmetros inválidos!\n"
                   "Informe os nomes dos arquivos de origem e destino.\n"
                   "Ex: ./filecopy <arquivo_origem.txt> <arquivo_destino.txt>\n"
                   "Obs.: Se o arquivo conter espaço, utilize aspas duplas para selecioná-lo.\n"
                   "Ex.: ./filecopy <\"Arquivo de origem.txt\"> <\"Arquivo de destino.txt\">\n");
            break;
        case WRITE_ERROR:
            printf("Erro: %d\n", errno);
            printf("%s: não foi possível abrir o arquivo %s\n", PROGRAM_NAME, file_names.destination);
            perror("causa");
//            printf("%s: Ocorreu um problema na escrita do arquivo %s: %s\n", PROGRAM_NAME, file_names.destination
//                   , "Mensagem do erro");
//            perror("causa");
            break;
        case OPEN_ERROR:
            printf("%s: Ocorreu um problema na leitura do arquivo %s\n", PROGRAM_NAME, file_names.source);
            break;
        default:
            exit(EXIT_FAILURE);
    }
}

/* Função que conta o número de bites de um dado arquivo.
 * @source: arquivo que será lido
 * @buffer: --
 * */
int countFileSize(char *source, char *buffer) {
    buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));
    int file_to_read = open(source, O_RDONLY);
    int size_bytes = read(file_to_read, buffer, SIZE_BUFFER);
    close(file_to_read);
    return size_bytes;
}
/* Função que verifica a leitura de um dado arquivo. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileRead(int file, char *source) {
    if ((file = open(source, O_RDONLY)) == -1) {
        printf("Erro: %d\n", errno);
        printf("%s: não foi possível abrir o arquivo %s\n", PROGRAM_NAME, source);
        perror("causa");
        print_errors(OPEN_ERROR);
        exit(EXIT_FAILURE);
    }
    return file;
}
/* Função que verifica se um dado arquivo possui permissão de escrita. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileWrite(int file, char *source) {
    if ((file = open(source, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == 1) {
        print_errors(WRITE_ERROR);
        exit(EXIT_FAILURE);
    }
    return file;
}

/* Função que realiza a cópia do conteúdo de um arquivo de origem para um arquivo de destino.
 * @source_file: arquivo que terá o conteúdo copiado (origem).
 * @destination_file: arquivo de destino.
 */
int writeFile(int source_file, int destination_file, char *buffer) {
    int n;
    while ((n = read(source_file, buffer, SIZE_BUFFER)) > 0)
        /* Caso o número de bytes retornado pela função write for diferente de n (quantidade de bytes),
         * o programa é encerrado e uma mesagem com o erro é exibida para o usuário (função print_result)
         */
        if (write(destination_file, buffer, n) != n) {
            print_errors(WRITE_ERROR);
            exit(EXIT_FAILURE);
        }

    if (n == -1) {
        print_errors(OPEN_ERROR);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    PROGRAM_NAME = argv[0]; // nome do programa recebido como primeiro parâmetro (argv[0])
    file_names.source = argv[1]; // nome do arquivo de origem recebido como segundo parâmetro (argv[1])
    file_names.destination = argv[2]; // nome do arquivo de origem recebido como segundo parâmetro (argv[2])
    int file_source, file_destination; // Variáveis que receberão os arquivos que serão utilizados
    char *buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));

    /* Verificando entrada de argumentos */
    if (argc != 3) {
        print_errors(INVALID_PARAMS);
        exit(EXIT_FAILURE);
    }

    // Verifica o arquivo de origem para tratar os possíveis erros de leitura
    file_source = checkFileRead(file_source, file_names.source);
    // Verifica o arquivo de destino para tratar os possíveis erros de leitura/escrita
    file_destination = checkFileWrite(file_destination, file_names.destination);

    /* Se não tiver nenhum problema nas etapas anteriores, então o conteúdo do arquivo de entrada é escrito
     * no arquivo de saída */
    writeFile(file_source, file_destination, buffer);

    /* conta o número de bites utilizando a função countFileSize */
    int size_bytes_file = countFileSize(file_names.source, buffer);

    /* Apresenta uma resposta amigável caso as operações anteriores executem sem erros (type SUCCESS)*/
    printf("%s: foram copiados %d bytes do arquivo %s para o arquivo %s\n", PROGRAM_NAME, size_bytes_file,
           file_names.source, file_names.destination);

    /* Fecha os arquivos de origem e destino */
    close(file_source);
    close(file_destination);
    exit(0);
}
