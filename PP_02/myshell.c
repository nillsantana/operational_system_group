/***************************************************************************//**
  @file         myshell.c
  @authors      Elivelton Rangel, Jane Barosa, Nilvan Santana, Mariese Conceição
  @brief        UNIVERSIDADE FEDERAL DA BAHIA - UFBA
                Projeto prático 02: Processos e Shell
                Sistemas Operacionais (MAT566)
*******************************************************************************/

#include "error_handles.c"

#define MAX_CHAR 4096
#define MAX_JOBS 10

/*
  Global variables
 */
char *PROGRAM_NAME;

int timeout = 0;
int child_done = 0;

/**
  Implementações de funções integradas.
*/

/**
   @brief Executa o comando help (mostra todas as funções disponíveis).
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_help(struct command *cmd) {
    int i;
    printf("Utilize as seguintes funções para continuar:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf("  %s\n", command_str[i]);
    }
    return 1;
}


/**
   @brief Executa o comando chdir.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_chdir(struct command *cmd) {
    if (cmd->arg_num <= 1) {
        printf("parâmetros insuficientes...\n");
        return 1;
    }
    printf("%s", cmd->args[1]);
    if (chdir(cmd->args[1]) == -1) {
        printf("%s: erro ao executar o comando %s: %s\n", PROGRAM_NAME, cmd->args[0], getMessageFromID(errno));
    }
    return 1;
}

#define PWD_BUFF 1024

/**
   @brief Executa o comando pwd.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_pwd(struct command *cmd) {
    char buff[PWD_BUFF];
    printf("%s\n", getwd(buff));
//    system("pwd");
    return 1;
}

/**
   @brief Executa o comando watchdog.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Retorna 0 e finaliza a execução do programa.
 */
int cmd_exit(void) {
    return 0;
}

#define SIZE_BUFFER 4096 //4kb
/**
   @brief Executa o comando start.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_start(struct command *cmd) {
    if (cmd->arg_num < 2) {
        printf("%s: número de argumentos incorretos: start <commands> \n", PROGRAM_NAME);
        return 1;
    }
    struct command new_cmd = {.arg_num=0, .args=malloc(sizeof(char *) * SIZE_BUFFER + 1)};
    int i;
    for (i = 1; i < cmd->arg_num; i++) {
        new_cmd.args[i - 1] = cmd->args[i];
    }
    new_cmd.args[cmd->arg_num - 1] = NULL;
    new_cmd.arg_num = cmd->arg_num - 1;
    return exec_command(&new_cmd);
}

/**
   @brief Executa o comando run.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_run(struct command *cmd) {
    struct command new_cmd = {.arg_num=0, .args=malloc(sizeof(char *) * SIZE_BUFFER + 1)};
    int i;
    for (i = 1; i < cmd->arg_num; i++) {
        new_cmd.args[i - 1] = cmd->args[i];
    }
    new_cmd.args[cmd->arg_num - 1] = NULL;
    new_cmd.arg_num = cmd->arg_num - 1;
    return exec_command_run(&new_cmd);
}

/**
   @brief Executa o comando wait.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_wait(struct command *cmd) {
    int wpid, status;
    wpid = waitpid(-1, &status, 0);
    if (wpid == -1) {
        printf("%s: não há processos restantes\n", PROGRAM_NAME);
        return 1;
    }
    handle_error(status, wpid);
    return 1;
}

/**
   @brief Executa o comando waitfor.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_wait_for(struct command *cmd) {
    int wpid, status;
    if (cmd->arg_num < 2) {
        printf("%s: número de argumentos incorretos: waitfor <pid> \n", PROGRAM_NAME);
        return 1;
    }
    pid_t pid = atoi(cmd->args[1]);
    wpid = waitpid(pid, &status, 0);
    if (wpid == -1) {
        printf("%s: processo inexistente\n", PROGRAM_NAME);
        return 1;
    }
    handle_error(status, pid);
    return 1;
}

/**
   @brief Executa o comando watchdog.
   @param args List of args.  Not examined.
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return Sempre retorna 1 para continuar a execução.
 */
int cmd_watchdog(struct command *cmd) {
    if (cmd->arg_num < 3) {
        printf("%s: número de argumentos incorretos: watchdog <limit_time> <command> \n", PROGRAM_NAME);
        return 1;
    }

    int limit_time = atoi(cmd->args[1]);
    struct command new_cmd = {.arg_num=0, .args=malloc(sizeof(char *) * SIZE_BUFFER + 1)};
    int i;
    for (i = 2; i < cmd->arg_num; i++) {
        new_cmd.args[i - 2] = cmd->args[i];
    }
    return exec_command_wd(&new_cmd, limit_time);
}


/**
  @brief Exibe um erro baseado no status de um processo.
  @param status Status do processo.
  @param pid Identificador do processo.
  @return Without return
 */
void handle_error(int status, int pid) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0)
            printf("%s: processo %d finalizou normalmente com o status %d\n", PROGRAM_NAME, pid, WEXITSTATUS(status));
        else
            printf("%s: processo %d finalizou de forma anormal com o sinal %d: %s\n", PROGRAM_NAME, pid,
                   WEXITSTATUS(status),
                   strsignal(WEXITSTATUS(status)));
    } else if (WIFSIGNALED(status)) {
        printf("%s: processo %d finalizou de forma anormal com o sinal %d: %s\n", PROGRAM_NAME, pid, WTERMSIG(status),
               strsignal(WTERMSIG(status)));
    } else if (WIFSTOPPED(status)) {
        printf("%s: processo %d finalizou de forma anormal com o sinal %d: %s\n", PROGRAM_NAME, pid, WTERMSIG(status),
               strsignal(WTERMSIG(status)));
    } else if (WIFCONTINUED(status)) {
        printf("%s: continua a execução\n", PROGRAM_NAME);
    }
}

/**
 @brief Manipulador de um processo filho para a função watchdog
 @param sig - sinal do sistema
 @return Sem retorno
*/
void child_handler(int sig) {
    child_done = 1;
}

/**
  @brief Manipulador de alarme para a função watchdog
  @param sig - sinal do sistema
  @return Sem retorno
 */
void alarm_handler(int sig) {
    timeout = 1;
}

/**
  @brief Inicia um programa e espera que ele termine (específico para função start).
  @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
  @return Sempre retorna 1 para continuar a execução
 */
int exec_command(struct command *cmd) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        printf("%s: processo %d iniciado\n", PROGRAM_NAME, getpid());
        int err = execvp(cmd->args[0], cmd->args);
        printf("erro %d", err);
        if ( err == -1) {
            printf("[%d] - %s\n", errno, getMessageFromID(errno));
        }
    } else if (pid < 0) {
        printf("%s: erro ao executar o comando %s: %s\n", PROGRAM_NAME, cmd->args[0], getMessageFromID(errno));
    }
    return 1;
}

/**
  @brief Inicia um programa e espera que ele termine (específico para função run).
  @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
  @return Sempre retorna 1 para continuar a execução
 */
int exec_command_run(struct command *cmd) {
    pid_t pid, wpid;
    int status, err;

    pid = fork();
    if (pid == 0) {
        if (execvp(cmd->args[0], cmd->args) == -1) {
            printf("[%d] - %s\n", errno, getMessageFromID(errno));
        }
    } else if (pid < 0) {
        printf("[%d] - %s\n", errno, getMessageFromID(errno));
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
            if (wpid == -1) {
                printf("%s: processo %d finalizou de forma anormal com o sinal %d: %s\n", PROGRAM_NAME, pid,
                       WEXITSTATUS(status),
                       strsignal(WEXITSTATUS(status)));
            }
            handle_error(status, pid);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

/**
  @brief Inicia um programa e espera que ele termine (específico para função watchdog).
  @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
  @return Sempre retorna 1 para continuar a execução
 */
int exec_command_wd(struct command *cmd, int limit_time) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(cmd->args[0], cmd->args) == -1) {
            printf("[%d] - %s\n", errno, getMessageFromID(errno));
        }
    } else if (pid < 0) {
        printf("[%d] - %s\n", errno, getMessageFromID(errno));
    }

    signal(SIGALRM, alarm_handler);
    signal(SIGCHLD, child_handler);

    alarm(limit_time);
    pause();

    if (timeout) {
        if (waitpid(pid, NULL, WNOHANG) == 0) {
            printf("%s: finalizando o processo %d, pois excedeu o tempo limite [%d]s\n", PROGRAM_NAME, pid, limit_time);
            kill(pid, 9);
            wait(&status);
            handle_error(status, pid);
        } else {
            wait(&status);
            printf("%s: o tempo limite foi excedido mas o processo %d foi finalizado normalmente. statuscode[%d]\n",
                   PROGRAM_NAME, pid, WEXITSTATUS(status));
        }
    } else if (child_done) {
        wait(&status);
        printf("%s: processo %d finalizou normalmente com o status %d\n", PROGRAM_NAME, pid, WEXITSTATUS(status));
    }
    return 1;
}

/**
   @brief Executa o programa com base no primeir argumento @arg[0]
   @param *cmd Estrutura do comando contendo o número de argumentos e um vetor de argumentos.
   @return 1 se o shell deve continuar rodando, 0 se deve finalizar o programa.
 */
int exec_shell(struct command *cmd) {
    int i;

    if (*cmd->args == NULL) {
        // Se não houver comandos, continua.
        return 1;
    }

    if (strcmp(cmd->args[0], "exit") == 0 || strcmp(cmd->args[0], "quit") == 0) {
        cmd_exit();
    } else if (strcmp(cmd->args[0], "start") == 0) {
        return cmd_start(cmd);
    } else if (strcmp(cmd->args[0], "run") == 0) {
        return cmd_run(cmd);
    } else if (strcmp(cmd->args[0], "wait") == 0) {
        return cmd_wait(cmd);
    } else if (strcmp(cmd->args[0], "waitfor") == 0) {
        return cmd_wait_for(cmd);
    } else if (strcmp(cmd->args[0], "watchdog") == 0) {
        return cmd_watchdog(cmd);
    } else if (strcmp(cmd->args[0], "chdir") == 0) {
        return cmd_chdir(cmd);
    } else if (strcmp(cmd->args[0], "pwd") == 0) {
        return cmd_pwd(cmd);
    } else if (strcmp(cmd->args[0], "help") == 0) {
        return cmd_help(cmd);
    } else {
        printf("%s: comando desconhecido: %s\n", PROGRAM_NAME, cmd->args[0]);
    }
    return 1;
}

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\n"

/**
   @brief traçar uma linha em tokens (muito ingenuamente).
   @param line linha de entrada.
   @return Número de argumentos de @line.
 */
int input_split(char *line, struct command *cmds) {
    int i = 0;
    char **commands = malloc(TOK_BUFSIZE * sizeof(char *) + 1);
    char *input_line;

    input_line = strtok(line, TOK_DELIM);
    while (input_line != NULL) {
        commands[i] = malloc(TOK_BUFSIZE * sizeof(input_line) + 1);
        commands[i] = input_line;
        i++;
        input_line = strtok(NULL, TOK_DELIM);
    }
    commands[i] = NULL;
    cmds->arg_num = i;
    cmds->args = commands;
    return i;
}

#define MAX_COMMANDS 100
#define MAX_INPUT 100

/**
   @brief Método prinipal.
   @param argc contador de argumentos.
   @param argv vetor de argumentos.
   @return código de status
 */
int main(int argc, char **argv) {
    int status = 1;
    char *fg_result;
    char input[MAX_INPUT] = "";
    struct command cmd = {.arg_num=0, .args=malloc(sizeof(char *) * SIZE_BUFFER + 1)};
    int len_input;

    /* Inicializa algumas variáveis*/
    PROGRAM_NAME = argv[0];

    while (status) {
        sleep(1); //bug de atraso do fork
        printf("myshell> ");
        fg_result = fgets(input, sizeof(input), stdin); //Recebe os parâmetros do usuário
        fflush(stdout);
        len_input = input_split(input, &cmd);

        //Verifica se a entrada é nula (para entrada de comandos com arquivos)
        if (fg_result == NULL) {
            return EXIT_FAILURE;
        }
        //Verifica se há argumentos para executar, caso negativo, disponibiliza novamente a entrada
        if (len_input < 1) {
            continue;
        }
        //Verifica se atingiu o número máximo de argumentos
        if (len_input > MAX_INPUT) {
            printf("%s: o número de argumentos não pode passar de 100\n", PROGRAM_NAME);
            continue;
        }
        // Executa os comandos digitados no terminal ou a partir da entrada por um arquivo de comandos
        status = exec_shell(&cmd);
    }
    return EXIT_SUCCESS;
}
