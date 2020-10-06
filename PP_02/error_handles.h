//
// Created by elivelton on 9/29/20.
//

#ifndef ERROR_HANDLES_H
#define ERROR_HANDLES_H

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/**
  Declaração da estrutura de comandos
 */
struct command {
    int arg_num;                            /* número de argumentos */
    char **args;                            /* array de argumentos */
};


/**
  Declarações de função para comandos shell integrados:
 */

int cmd_help(struct command *cmd);

int cmd_exit(void);

int cmd_chdir(struct command *cmd);

int cmd_pwd(struct command *cmd);

int cmd_start(struct command *cmd);

int cmd_wait(struct command *cmd);

int cmd_wait_for(struct command *cmd);

int cmd_run(struct command *cmd);

int cmd_watchdog(struct command *cmd);

int exec_shell(struct command *cmd);

int exec_command_run(struct command *cmd);

int exec_command_wd(struct command *cmd, int limit_time);

int exec_command(struct command *cmd);

void alarm_handler(int);

void child_handler(int);

void handle_error(int status, int pid);


/**
  Lista de comandos embutidos, seguidos de suas funções correspondentes.
 */
char *command_str[] = {
        "help",
        "exit",
        "pwd",
        "chdir",
        "start",
        "wait",
        "waitfor",
        "run",
        "watchdog",
};

int num_builtins() {
    return sizeof(command_str) / sizeof(char *);
}

typedef struct error_handle {
    int code;
    char *original_message;
    char *translated_message;
    char *category;
} ErrorHandle;

/**
   @brief Apresenta uma mensagem amigável a partir de uma falha de algum processo (errno)
   @param identificador do erro (variável global de erro - errno)
   @return Retorna a mensagem amigável
 */
char* getMessageFromID (int error_id);

#endif //PROJECT_SO_01_ERROR_HANDLES_H
