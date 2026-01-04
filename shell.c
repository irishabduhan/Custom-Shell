#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_CMD 100
#define MAX_ARGS 20

/* ---------- Utility ---------- */

void print_prompt() {
    printf("shell>> ");
    fflush(stdout);
}

bool read_command(char *cmd) {
    if (!fgets(cmd, MAX_CMD, stdin))
        return false;
    cmd[strcspn(cmd, "\n")] = '\0';
    return true;
}

void trim_trailing_spaces(char *cmd) {
    int len = strlen(cmd);
    while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\t')) {
        cmd[--len] = '\0';
    }
}

bool check_background(char *cmd) {
    int len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '&') {
        cmd[--len] = '\0';
        trim_trailing_spaces(cmd);
        return true;
    }
    return false;
}

void tokenize(char *cmd, char **args) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

/* ---------- Built-ins ---------- */

bool handle_builtin(char **args) {
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        return true;
    }

    if (strcmp(args[0], "cd") == 0) {
        if (!args[1])
            fprintf(stderr, "cd: missing operand\n");
        else if (chdir(args[1]) != 0)
            perror("cd");
        return true;
    }

    if (strcmp(args[0], "mkdir") == 0) {
        if (!args[1])
            fprintf(stderr, "mkdir: missing operand\n");
        else if (mkdir(args[1], 0755) != 0)
            perror("mkdir");
        return true;
    }

    if (strcmp(args[0], "help") == 0) {
        printf("Supported commands:\n");
        printf("pwd\ncd <dir>\nmkdir <dir>\nls <flags>\nexit\nhelp\n");
        return true;
    }

    return false;
}

/* ---------- Execute without pipe ---------- */

void execute_simple(char **args, bool background) {
    pid_t pid = fork();

    if (pid == 0) {
        if (!handle_builtin(args)) {
            execvp(args[0], args);
            perror("command not found");
        }
        exit(0);
    }

    if (!background)
        waitpid(pid, NULL, 0);
}

/* ---------- Execute with single pipe ---------- */

void execute_pipe(char *left, char *right, bool background) {
    int fd[2];
    pipe(fd);

    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];

    tokenize(left, args1);
    tokenize(right, args2);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(args1[0], args1);
        perror("left command failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        execvp(args2[0], args2);
        perror("right command failed");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    if (!background) {
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
}

/* ---------- Main ---------- */

int main() {
    char cmd[MAX_CMD];

    while (true) {
        print_prompt();

        if (!read_command(cmd))
            break;

        trim_trailing_spaces(cmd);
        if (strlen(cmd) == 0)
            continue;

        if (!strcmp(cmd, "exit") || !strcmp(cmd, "Exit"))
            break;

        bool background = check_background(cmd);

        char *pipe_pos = strchr(cmd, '|');

        if (pipe_pos) {
            *pipe_pos = '\0';
            char *left = cmd;
            char *right = pipe_pos + 1;

            trim_trailing_spaces(left);
            trim_trailing_spaces(right);

            execute_pipe(left, right, background);
        }
        else {
            char *args[MAX_ARGS];
            tokenize(cmd, args);
            execute_simple(args, background);
        }
    }

    return 0;
}
