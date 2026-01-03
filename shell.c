#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>

int main() {
    pid_t pid;

    while (true) {
        printf("shell>> ");
        fflush(stdout);

        char cmd[100];
        fgets(cmd, sizeof(cmd), stdin);

        // remove newline
        cmd[strcspn(cmd, "\n")] = '\0';

        // remove trailing spaces
        int len = strlen(cmd);
        while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\t')) {
            cmd[--len] = '\0';
        }

        if (len == 0)
            continue;

        // check background execution
        bool background = false;
        if (cmd[len - 1] == '&') {
            background = true;
            cmd[--len] = '\0';

            // remove spaces before &
            while (len > 0 && cmd[len - 1] == ' ') {
                cmd[--len] = '\0';
            }
        }

        // exit command (parent handled)
        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "Exit") == 0) {
            break;
        }

        // tokenize command
        char *args[20];
        int i = 0;
        char *token = strtok(cmd, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid = fork();

        if (pid < 0) {
            perror("fork failed");
        }
        else if (pid == 0) {
            /* ================= CHILD PROCESS ================= */

            // pwd
            if (strcmp(args[0], "pwd") == 0) {
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    printf("%s\n", cwd);
                else
                    perror("pwd");
                exit(0);
            }

            // cd (must be here for assignment compliance)
            if (strcmp(args[0], "cd") == 0) {
                if (args[1] == NULL)
                    fprintf(stderr, "cd: missing operand\n");
                else if (chdir(args[1]) != 0)
                    perror("cd");
                exit(0);
            }

            // mkdir
            if (strcmp(args[0], "mkdir") == 0) {
                if (args[1] == NULL)
                    fprintf(stderr, "mkdir: missing operand\n");
                else if (mkdir(args[1], 0755) != 0)
                    perror("mkdir");
                exit(0);
            }

            // help
            if (strcmp(args[0], "help") == 0) {
                printf("Supported commands:\n");
                printf("pwd\n");
                printf("cd <directory>\n");
                printf("mkdir <directory>\n");
                printf("ls <flags>\n");
                printf("exit\n");
                printf("help\n");
                printf("Executable commands are also supported\n");
                exit(0);
            }

            // ls and any other executable
            execvp(args[0], args);
            perror("command not found");
            exit(1);
        }
        else {
            /* ================= PARENT PROCESS ================= */
            if (!background) {
                waitpid(pid, NULL, 0);
            }
        }
    }
    
    return 0;
}
