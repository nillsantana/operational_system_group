//
// Created by elivelton on 9/18/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>


extern int errno;

char *program_name;
long size_buffer = 4096;
long count_files = 0, count_dir = 0, count_bytes = 0;

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

int checkDir(char *source, char *destination) {
    struct dirent *entry;
    unsigned long len_source = strlen(source);
    unsigned long len_destination = strlen(destination);

    DIR *dir_source = opendir(source);
//    DIR *dir_destination = opendir(destination);

    if (dir_source == NULL) {
        closedir(dir_source);
        printf("Erro ao tentar abrir o diretório de origem: %s\n", source);
        perror("Causa: ");
        exit(EXIT_FAILURE);
    }

//    if (dir_destination != NULL) {
//        perror("O diretório de origem já existe");
//        exit(EXIT_FAILURE);
//    } else {
//        int r = mkdir(destination, 0777);
//        if (r == -1){
//            perror("Erro ao criar o diretório de origem: ");
//            exit(EXIT_FAILURE);
//        }
//        dir_destination = opendir(destination);
////        len_destination = strlen(destination);
//    }

//    puts(source);
//    puts(destination);
    while ((entry = readdir(dir_source)) != NULL) {
        char *name = entry->d_name;
        int type = entry->d_type;
        if (!(strcmp(name, ".")) || !strcmp(name, ".."))
            continue;
        if (type == DT_DIR) { //é diretório
            count_dir++;
            source[len_source] = '/';
            destination[len_destination] = '/';

            strcpy(source + len_source + 1, name);
            strcpy(destination + len_destination + 1, name);

            printf("%s, %s\n", source, destination);
            checkDir(source, destination);

            source[len_source] = '\0';
//            destination[len_destination] = '\0';

        } else if (type == DT_REG) { //é um arquivo regular
            count_files++;
            count_bytes = count_files * 4;
            printf("copiando %s/%s -> %s/%s\n", source, name, destination, name);
        } else {
            printf("Não é nem diretório nem arquivo: %s/%s", source, name);
        }
    }
    closedir(dir_source);
//    closedir(dir_destination);

    if (errno == ENOTDIR)
        return 1;
    return -1;
}

int main(int argc, char **argv) {
    program_name = argv[0];
    int directories[2];

    //    Verificando entrada de argumentos (não pode ser diferente de 2 - arquivo_origem arquivo_destino)
    if (argc != 3) {
        perror("Necessário ter um diretório de origem e outro de destino para continuar...");
        exit(0);
    }

    DIR *dir_src = opendir(argv[1]);
    if (dir_src == NULL) {
        perror("Ocorreu um erro ao tentar encontrar o diretorio");
        exit(0);
    }
    checkDir(argv[1], argv[2]);
    closedir(dir_src);

    printf("\n\n%s: Foram copiados %ld diretórios, %ld arquivos e %ld bytes de %s para %s.\n", program_name, count_dir,
           count_files, count_bytes, argv[1], argv[2]);
    exit(0);
}


