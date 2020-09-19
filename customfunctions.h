/*Header*/
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
    char** original_message;
    char** translated_message;
    char** category;
} ErrorHandle;

/*
 * A estrutura FileManagement organiza os arquivos de origem e destino para o tratamento de suas respectivas operacoes
 */
typedef struct file_management {
    char** file_src;
    char** file_dst;
} FileManagement;

/* A função checkFiles verifica se os arquivos existem no SO */
FileManagement checkFiles(char** files);

/* A função checkFiles verifica se os arquivos existem no SO */
FileManagement openFiles(char** file);

/* A função writeFile Escreve os dados de um buffer no arquivo */
void writeFile(int source_file, int destiny_file, char *buffer);


#endif /* CUSTOMFUNCTIONS_H */