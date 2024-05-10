#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Function prototypes
void execute_command(char *args[]);
int cd(char *path);
void pwd();
void echo(char *args[]);

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("$ ");
        fflush(stdout);

        // Read the user input
        if (!fgets(input, MAX_INPUT, stdin)) {
            printf("\n");
            break;
        }

        // Tokenize input
        char *token;
        int i = 0;
        token = strtok(input, " \n");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;

        // Check for built-in commands
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: expected argument to \"cd\"\n");
            } else {
                if (cd(args[1]) != 0) {
                    perror("cd");
                }
            }
        } else if (strcmp(args[0], "pwd") == 0) {
            pwd();
        } else if (strcmp(args[0], "echo") == 0) {
            echo(args);
        } else if (strcmp(args[0], "exit") == 0) {
            break;
        } else {
            execute_command(args);
        }
    }

    return 0;
}

// Execute external command
void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

// Change directory
int cd(char *path) {
    return chdir(path);
}

// Print current working directory
void pwd() {
    char cwd[MAX_INPUT];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}

// Echo command
void echo(char *args[]) {
    int i = 1;
    while (args[i] != NULL) {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
}

