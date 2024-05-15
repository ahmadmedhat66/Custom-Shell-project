#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64


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

        
        if (!fgets(input, MAX_INPUT, stdin)) {
            printf("\n");
            break;
        }

        
        char *token;
        int i = 0;
        token = strtok(input, " \n");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;

        
        if (args[0] == NULL) {
            continue;
        } else if (strcmp(args[0], "cd") == 0) {
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


void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        
        int status;
        waitpid(pid, &status, 0);
    }
}


int cd(char *path) {
    return chdir(path);
}


void pwd() {
    char cwd[MAX_INPUT];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}


void echo(char *args[]) {
    int i = 1;
    int redirect_index = -1;
    int append_mode = 0;

    
    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0) {
            redirect_index = i;
            append_mode = 0;
            break;
        } else if (strcmp(args[i], ">>") == 0) {
            redirect_index = i;
            append_mode = 1;
            break;
        }
        i++;
    }

    if (redirect_index != -1 && args[redirect_index + 1] != NULL) {
        
        FILE *file;
        if (append_mode) {
            file = fopen(args[redirect_index + 1], "a");
        } else {
            file = fopen(args[redirect_index + 1], "w");
        }

        if (file == NULL) {
            perror("fopen");
            return;
        }

        
        for (i = 1; i < redirect_index; i++) {
            fprintf(file, "%s ", args[i]);
        }
        fprintf(file, "\n");
        fclose(file);
    } else {
        
        i = 1;
        while (args[i] != NULL) {
            printf("%s ", args[i]);
            i++;
        }
        printf("\n");
    }
}
