/* Universidade Federal da Bahia
 * Sistemas Operacionais (MATA58). Semestre Letivo Suplementar 2020
 * Componentes: Elivelton Rangel, Jane (sobrenome), Mariese (sobrenome), Nilvan Santana
 * Exercício prático parte 2: Cópia recursiva de uma árvore de diretórios
 * Programa que cria uma cópia exata de um diretório DiretorioOrigem no novo diretório DiretorioDestino.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int errno;

long SIZE_BUFFER = 4096; //Tamanho do buffer é de 4kb
char *PROGRAM_NAME;
long COUNT_DIR, COUNT_FILES, COUNT_BYTES_SIZE;

enum result_types_ref {
    INVALID_PARAMS,
    INVALID_PARAM_DIR,
    OPEN_ERROR,
    OPEN_DIR_ERROR,
    CREATE_DIR_ERROR,
    WRITE_DIR_ERROR,
    WRITE_ERROR,
    READ_ERROR,
    SUCCESS_FILE,
    SUCCESS_DIR
} type;

//Estrutura para armazenar os arquivos de origem e destino passados por parâmetro (linha de código)
typedef struct file_dir_management {
    char *source;
    char *destination;
} FileDirManagement;

FileDirManagement dir_names;

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
        case INVALID_PARAM_DIR:
            perror("Parâmetros inválidos!\n"
                   "Informe os nomes dos diretórios de origem e destino.\n"
                   "Ex: ./treecopy <DiretorioOrigem> <DiretorioDestino>\n"
                   "Obs.: Se o diretório conter espaço, utilize aspas duplas para selecioná-lo.\n"
                   "Ex.: ./treecopy <\"Diretório de origem.txt\"> <\"Diretório de destino.txt\">\n");
            break;
        case WRITE_ERROR:
            printf("Erro: %d\n", errno);
            printf("%s: não foi possível abrir o arquivo %s\n", PROGRAM_NAME, dir_names.destination);
            perror("causa");
//            printf("%s: Ocorreu um problema na escrita do arquivo %s: %s\n", PROGRAM_NAME, dir_names.destination
//                    , "Mensagem do erro");
//            perror("causa");
            break;
        case OPEN_ERROR:
            printf("%s: Ocorreu um problema na leitura do arquivo %s\n", PROGRAM_NAME, dir_names.source);
            break;
        case CREATE_DIR_ERROR:
            perror("Erro ao criar o diretório de origem: ");
            break;
        case OPEN_DIR_ERROR:
            printf("Erro ao tentar abrir o diretório de origem: %s\n", dir_names.source);
            perror("Causa: ");
            break;
        case WRITE_DIR_ERROR:
            printf("O diretório de origem já existe!");
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
void writeFile(int source_file, int destination_file, char *buffer) {
    int n;
    while ((n = read(source_file, buffer, SIZE_BUFFER)) > 0)
        /* Caso o número de bytes retornado pela função write for diferente de n (quantidade de bytes),
         * o programa é encerrado e uma mesagem com o erro é exibida para o usuário (função print_errors)
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

/* Função que verifica os arquivos existentes de um dado diretório de origem para copiá-los em um diretório de destino
 * @source: Diretório de origem
 * @destination: Diretório de destino
 */
void checkDir(char *source, char *destination) {
    struct dirent *entry;
    int file_source, file_destination; // Variáveis que receberão os arquivos que serão utilizados

    // Guarda o tamanho do nome dos diretórios
    unsigned long len_source = strlen(source);
    unsigned long len_destination = strlen(destination);

    /* Cria as variáveis temporárias */
    char *source_tmp = calloc(len_source + 1, sizeof(source));
    char *destination_tmp = calloc(len_destination + 1, sizeof(destination));

    /* Faz a cópia dos nomes dos diretórios de origem e destino para as variáveis temporárias */
    strcpy(source_tmp, source);
    strcpy(destination_tmp, destination);

    DIR *dir_source = opendir(source);
    DIR *dir_destination = opendir(destination);

    /* Verifica se o diretório de origem é nulo, caso verdade, apresenta um erro */
    if (dir_source == NULL) {
        closedir(dir_source);
        print_errors(OPEN_DIR_ERROR);
        exit(EXIT_FAILURE);
    }
    /* Verifica se o diretório de destino existe, caso verdade, apresenta um erro */
    if (dir_destination != NULL) {
        print_errors(WRITE_DIR_ERROR);
        exit(EXIT_FAILURE);
    } else {
        int r = mkdir(destination, 0777); // Cria um diretório com as todas as permissões
        if (r == -1) { // Se houver algum problema na criação do diretório, apresenta um erro.
            print_errors(CREATE_DIR_ERROR);
            exit(EXIT_FAILURE);
        }
    }

    // Enquanto possuir conteúdo dentro do diretório, o laço é verdadeiro
    while ((entry = readdir(dir_source)) != NULL) {
        char *name = entry->d_name; // Recebe o nome do diretório/arquivo atual
        int dType = entry->d_type; // Recebe o tipo de entrada (diretório, arquivo ou qualquer outra coisa)
        char *buffer = (char *) calloc(SIZE_BUFFER, sizeof(char));
        if (!(strcmp(name, ".")) || !strcmp(name, "..")) // Desconsidera o diretório atual e o anterior
            continue;
        if (dType == DT_DIR) { //é diretório
            COUNT_DIR++;
            //Monta os caminhos dos novos diretórios
            source_tmp[len_source] = '/';
            destination_tmp[len_destination] = '/';

            strcpy(source_tmp + len_source + 1, name);
            strcpy(destination_tmp + len_destination + 1, name);

            // Após definir os caminhos na etapa anterior, entra de forma recursiva para copiar os conteúdos.
            checkDir(source_tmp, destination_tmp);

            // Volta para o diretório anterior, removendo as informações do diretório atual.
            source_tmp[len_source] = '\0';
            destination_tmp[len_destination] = '\0';

        } else if (dType == DT_REG) { //é um arquivo regular
            COUNT_FILES++;

            //Monta os caminhos dos arquivos
            source_tmp[len_source] = '/';
            destination_tmp[len_destination] = '/';

            strcpy(source_tmp + len_source + 1, name);
            strcpy(destination_tmp + len_destination + 1, name);

            //Verifica se é possível realizar a leitura do arquivo de origem e a escrita no arquivo de destino
            file_source = checkFileRead(file_source, source_tmp);
            file_destination = checkFileWrite(file_destination, destination_tmp);
            /* caso a etapa anterior não apresente problemas, escreve o conteúdo do arquivo de origem no arquivo de destino */
            writeFile(file_source, file_destination, buffer);
            COUNT_BYTES_SIZE += countFileSize(source_tmp, buffer);
            printf("copiando %s -> %s\n", source_tmp, destination_tmp);
        } else { // Caso o tipo de entrada não seja esperada (arquivo ou diretório), finaliza a execução do programa.
            printf("Não é nem diretório nem arquivo: %s/%s", source, name);
            exit(EXIT_FAILURE);
        }
    }
    closedir(dir_source);
    closedir(dir_destination);
}

int main(int argc, char **argv) {
    PROGRAM_NAME = argv[0]; // nome do programa recebido como primeiro parâmetro (argv[0])
    dir_names.source = argv[1]; // nome do diretório de origem recebido como segundo parâmetro (argv[1])
    dir_names.destination = argv[2]; // nome do diretório de origem recebido como segundo parâmetro (argv[2])

    //    Verificando entrada de argumentos (não pode ser diferente de 2 - arquivo_origem arquivo_destino)
    if (argc != 3) {
        print_errors(INVALID_PARAM_DIR);
        exit(EXIT_FAILURE);
    }
    /* Executa a verificação dos diretórios */
    checkDir(dir_names.source, dir_names.destination);
    /* Se o processo anterior executar com sucesso, uma mensagem amigável é exibida com o reultado da operação */
    printf("\n\n%s: Foram copiados %ld diretórios, %ld arquivos e %ld bytes de %s para %s\n", PROGRAM_NAME,
           COUNT_DIR, COUNT_FILES, COUNT_BYTES_SIZE, dir_names.source, dir_names.destination);
    exit(0);
}
