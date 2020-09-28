#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_CHAR 100

/* Programa principal não recebe nenhum argumento */

enum type_entry_ref {
    start,
    run,
    wait,
    waitfor,
    watchdog,
    chdir,
    pwd,
    help
} TypeEntry;

char **extractCommand(char *commands) {
    char *command = strtok(commands, " ");
    char **arrCommands = malloc(TAM_CHAR * sizeof(char *));
//    char *arrCommands[TAM_CHAR];
    int nPalavras = 0;

    while (command != NULL) {
        printf("%s\n", command);
        int lenStr = sizeof(command);
        arrCommands[nPalavras] = (char *) malloc(sizeof(char) * lenStr + 1);
//        if (!arrCommands[nPalavras]) { //Se não conseguir alocar, para de executar
//            free(arrCommands);
//            return NULL;
//        }
//        arrCommands[nPalavras] = command;
//        strcpy(arrCommands[nPalavras], command); //Realiza a cópia da string para o array
        command = strtok(NULL, " ");
        nPalavras++;
    }
//    arrCommands[nPalavras] = 0;
//    strcpy(arrCommands[nPalavras], 0);
    return arrCommands;
}

int execCommand(enum type_entry_ref command, char** commands, int nWords) {
    int i;
    switch (command) {
        case start:
            printf("Comando START executado...\n");
            for (i = 0; i < nWords; i++){
                printf("Comando: %s\n", commands[i]);
            }
            break;
        case run:
            printf("Comando RUN executado...\n");
            if (nWords <=1){
                printf("Faltam parâmetros para o comando RUN...\n");
                return 0;
            }
            for (i = 1; i < nWords; i++){
                printf("Comando: %s\n", commands[i]);
            }
            break;
        case wait:
            printf("Comando WAIT executado...\n");
            break;
        case waitfor:
            printf("Comando WAITFOR executado...\n");
            break;
        case watchdog:
            printf("Comando WATCHDOG executado...\n");
            break;
        case chdir:
            printf("Comando CHDIR executado...\n");
            break;
        case pwd:
            break;
        case help:
            printf("Argumento inválido! Digite o que você deseja: \n"
                   "<start> - Executa um comando\n"
                   "<run> - Executa um comando\n"
                   "<wait> - Executa um comando\n"
                   "<waitfor> - Executa um comando\n"
                   "<watchdog> - Executa um comando\n"
                   "<chdir> - Muda o diretório corrente\n"
                   "<quit> ou <exit> - Finaliza o terminal\n"
                   "");
            break;
        default:
            execCommand(help, NULL, 0);
            return 0;
    }
}

char **splitCommand(char *command, int *nWords){
    char ** arr = malloc(TAM_CHAR * sizeof(char *));
    char *cmdLine = strtok(command, " \t\n");
//    if (strtok(cmdLine, "\n") == NULL)
//        return NULL;
    int i = 0;
    while (cmdLine != NULL) {
        arr[i] = malloc(TAM_CHAR * sizeof(cmdLine) + 1);
        arr[i] = cmdLine;
        cmdLine = strtok(NULL, " \t\n");
        i++;
//        *nWords = *nWords+1;
    }
    *nWords = i;
    return arr;
}

void main(int argc, char **argv) {
    char command[TAM_CHAR];// = malloc(100 * sizeof(char));
    char **commands;// = malloc(100 * sizeof(char));
    int nWords = 0;
    enum type_entry_ref type_r = help;

    while (1) {
        printf("myshell> ");
        fgets(command, sizeof(command), stdin);
        fflush(stdout);
//    printf("Valor inserido no terminal: %s", command);
//    commands = extractCommand(command);
        commands = splitCommand(command, &nWords);

        if (nWords > 0){
            printf("Comando: %s\n", commands[0]);
            if (strcmp(commands[0], "start") == 0) type_r = start;
            else if (strcmp(commands[0], "run") == 0) type_r = run;
            else if (strcmp(commands[0], "wait") == 0) type_r = wait;
            else if (strcmp(commands[0], "waitfor") == 0) type_r = waitfor;
            else if (strcmp(commands[0], "watchdog") == 0) type_r = watchdog;
            else if (strcmp(commands[0], "chdir") == 0) type_r = chdir;
            else if (strcmp(commands[0], "pwd") == 0) type_r = pwd;
            else if (strcmp(commands[0], "quit") == 0 || strcmp(commands[0], "exit") == 0) exit(0);
            else type_r = help;
            execCommand(type_r, commands, nWords);
        } else {
            execCommand(type_r, NULL, 0);
//        printf("%s", commands[5]);
        }
        free(commands);
//        free(command);
    }


//    if (strcmp(command, "exit") != 0 || strcmp(command, "quit") != 0) {
//        exit(0);
//    }

//    free(command);

//    while (1==1) {
//
//    }

//    exit(0);
}