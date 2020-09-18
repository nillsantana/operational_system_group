#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

extern int errno;

void print_result(int type, char** argv, char* buff, int limit_bites){
    /*Types:
     * 0 parâmetros inválidos
     * 9 successo
     * */

    buff = (char *) calloc(limit_bites, sizeof(char));
    int frd, sz;

    switch (type) {
        case 0:
            perror("Parâmetros inválidos!\n"
                   "Informe os nomes dos arquivos de origem e destino.\n"
                   "Ex: ./filecopy file_src.txt file_dest.txt");
            break;

        case 9:
            frd = open(argv[2], O_RDONLY);
            sz = read(frd, buff, limit_bites);
            printf("%s: foram copiados %d bites do arquivo %s para o arquivo %s\n", argv[0], sz, argv[1], argv[2]);
            break;
    }
}

typedef struct {
    int code;
    char original_message[10];
    char translated_message[10];
} ErrorHandles;

int main(int argc, char** argv)
{
    long limit_bites = 100;
    int file[2];
    char *buff = (char *) calloc(limit_bites, sizeof(char));
    ErrorHandles errors[129];

//    Verificando entrada de argumentos (não pode ser diferente de 2 - arquivo_origem arquivo_destino)
    if (argc != 3) {
        print_result(0, argv, buff, limit_bites);
        exit(0);
    }

    if((file[0] = open(argv[1], O_RDONLY)) == -1) {
        printf("Erro: %d\n", errno);
        printf("%s: não foi possível abrir o arquivo %s\n", argv[0], argv[1]);
        perror("causa");
        exit(1);
    }

    if((file[1] = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0700)) == 1){
        printf("%s: não foi possível abrir o arquivo %s\n", argv[0], argv[2]);
        perror("causa");
        exit(2);
    }

    while((n=read(file[0], buff, limit_bites))>0)

        if(write(file[1], buff, n)!=n)
        {
            printf("%s: Um problema ocorreu na escrita do arquivo %s\n", argv[0], argv[2]);
            perror("causa");
            exit(3);
        }

    if(n==-1)
    {
        printf("%s: Um problema ocorreu na leitura do arquivo %s\n", argv[0], argv[1]);
        exit(2);
    }

    print_result(9, argv, buff, limit_bites);
    close(file[1]);

    exit(0);
}