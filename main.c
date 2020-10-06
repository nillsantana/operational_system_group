#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "PP_02/customfunctions.c"

#define MAX_LINE 255

char **strsplit(char *str, char *delim){
    char ** arr = malloc(MAX_LINE * sizeof(char *));
    char *cmdLine = strtok(str, delim);
    int i = 0;
    while (cmdLine != NULL) {
        arr[i] = malloc(MAX_LINE * sizeof(cmdLine) + 1);
        arr[i] = cmdLine;
        cmdLine = strtok(NULL, delim);
        i++;
    }
    return arr;
};

char *strjoin(char const *s1, char const *s2);


int isexec(char *path) {
    struct stat buf;

    lstat(path, &buf);
    if (S_ISREG(buf.st_mode) && (S_IXUSR & buf.st_mode))
        return (1);
    return (0);
}

static char *find_exec_readdir(char *paths, char *cmd) {
    DIR *dir;
    struct dirent *dirent;
    char *exec;

    exec = NULL;
    if ((dir = opendir(paths)) != NULL) {
        while ((dirent = readdir(dir)) != NULL) {
            if (!strcmp(dirent->d_name, cmd)) {
                exec = strdup(dirent->d_name);
                break;
            }
        }
        if (closedir(dir))
            dprintf(2, "Failed closing dir.\n");
    }
    return (exec);
}

char *find_exec(char *cmd, char **paths) {
    char *exec;
    char *path;
    char *tmp;
    int i;

    i = -1;
    exec = NULL;
    path = NULL;
    if ((cmd[0] == '.' || cmd[0] == '/')) {
        if (isexec(cmd))
            return (strdup(cmd));
        return (NULL);
    }
    while (paths[++i])
        if ((exec = find_exec_readdir(paths[i], cmd)) != NULL) {
            tmp = strjoin(paths[i], "/");
            path = strjoin(tmp, exec);
            free(tmp);
            free(exec);
            break;
        }
    return (path);
}

int print_signal_error(int sig){
    return -1;
}

int handle_return_status(int status) {
    int sig;
    int i;

    if (!WIFEXITED(status) && WIFSIGNALED(status)) {
        sig = WTERMSIG(status);
        i = -1;
        while (++i <= 13) {
            if (print_signal_error(sig)) {
                return (-1);
            }
        }
        dprintf(2, "Process terminated with unknown signal: %d\n", sig, NULL);
        return (-1);
    }
    return (0);
}


int main(int argc, char *argv[]) {
    char *input = NULL;
    char **command = NULL;
    int should_run = 1;
    int status = 0;


    (void) argc;
    (void) argv;
    if ((input = (char *) malloc(MAX_LINE * sizeof(char))) == NULL)
        return (dprintf(2, "Failed to malloc, abort.\n"));
    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        pid_t pid;
        pid = fork();
        if (pid < 0)
            return (dprintf(2, "error with creating chiled process\n"));

        if (pid == 0) {
            fgets(input, MAX_LINE, stdin);
            command = strsplit(input, ' \t\n');
            command[0] = find_exec(command[0], strsplit(getenv("PATH"), ':'));

            if (execv(command[0], &command[1]) < 0)
                return (dprintf(2, "Error in execution.\n"));
            //should_run = 0;
        }
        waitpid(pid, &status, 0);
        handle_return_status(status);

    }
    return 0;
}