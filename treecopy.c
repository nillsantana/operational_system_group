//
// Created by elivelton on 9/18/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

extern int errno;

int main(int argc, char** argv){
    struct dirent *de;
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

    while ((de = readdir(dir_src)) != NULL){
        printf("%s -- ", de->d_name);
        printf("%c\n",de->d_type);

    }



    closedir(dir_src);
    exit(0);
}


