/* Arquivo header com as definições das estruturas e notações das funções utilizadas pelos programas filecopy e treecopy
 *
 */
#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

/*
 * A estrutura ErrorHandle organiza o tipo de erro para uma mensagem mais amigavel para o usuario
 * @code: identificador do erro
 * @original_message: mensagem original do erro
 * @translated_message: mensagem amigavel para o usuario
 * @category: tipo de erro atribuido a mensagem (open, close, read, write, etc)
 */
typedef struct error_handle {
    int code;
    char *translated_message;
    char *original_message;
    char *category;
} ErrorHandle;

//Estrutura de referência para o tipo de erro reportado
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

/* Função que exibe as mensagens amigáveis caso um erro ocorra.
 * @typesRef: identificador do tipo de erro a ser reportado
 * */
void print_errors(enum result_types_ref typesRef);

/* Função que conta o número de bites de um dado arquivo.
 * @source: arquivo que será lido
 * @buffer: --
 * */
int countFileSize(char *source, char *buffer);

/* Função que verifica a leitura de um dado arquivo. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileRead(int file, char *source);

/* Função que verifica se um dado arquivo possui permissão de escrita. Caso ocorra um problema, o programa é finalizado
 * @file: identificador do arquivo que será aberto.
 * @source: caminho ou arquivo que será aberto.
 */
int checkFileWrite(int file, char *source);

/* Função que realiza a cópia do conteúdo de um arquivo de origem para um arquivo de destino.
 * @source_file: arquivo que terá o conteúdo copiado (origem).
 * @destination_file: arquivo de destino.
 */
void writeFile(int source_file, int destination_file, char *buffer);

/* Função que verifica os arquivos existentes de um dado diretório de origem para copiá-los em um diretório de destino
 * @source: Diretório de origem
 * @destination: Diretório de destino
 */
void checkDir(char *source, char *destination);


#endif /* CUSTOMFUNCTIONS_H */