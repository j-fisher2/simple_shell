#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

extern char** environ;

// Function to set shell environment
void set_shell_environment() {
    pid_t pid = getpid();
    char path[256];
    sprintf(path, "/proc/%d/exe", pid);
    char buf[256];
    readlink(path, buf, sizeof(buf) - 1);
    setenv("shell", buf, 1);
}

// Function to execute internal commands
void execute_internal_command(char *command) {
    // Parse the command
    char *token = strtok(command, " ");

    if (token == NULL) {
        return; // Empty command
    }

    if (strcmp(token, "cd") == 0) {
        // Handle cd command
        token = strtok(NULL, " ");
        if (token == NULL) {
            // No argument, print current directory
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        } else {
            // Change directory
            if (chdir(token) != 0) {
                perror("cd");
            }
        }
    } else if (strcmp(token, "clr") == 0) {
        // Handle clr command
        system("clear");
    } else if (strcmp(token, "dir") == 0) {
        // Handle dir command
        token = strtok(NULL, " ");
        if (token == NULL) {
            // No argument, list current directory
            DIR *dir;
            struct dirent *entry;

            dir = opendir(".");
            if (dir == NULL) {
                perror("dir");
                return;
            }

            while ((entry = readdir(dir)) != NULL) {
                printf("%s\n", entry->d_name);
            }

            closedir(dir);
        } else {
            // List specified directory
            DIR *dir = opendir(token);
            if (dir == NULL) {
                perror("dir");
                return;
            }

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                printf("%s\n", entry->d_name);
            }

            closedir(dir);
        }
    } else if (strcmp(token, "environ") == 0) {
        // Handle environ command
        char **env = environ;
        while (*env != NULL) {
            printf("%s\n", *env);
            env++;
        }
    } else if (strcmp(token, "echo") == 0) {
        // Handle echo command
        token = strtok(NULL, "");
        if (token != NULL) {
            printf("%s\n", token);
        }
    } else if (strcmp(token, "help") == 0) {
        // Handle help command
        system("more readme");
    } else if (strcmp(token, "pause") == 0) {
        // Handle pause command
        printf("Press Enter to continue...");
        fflush(stdout);
        getchar(); // Wait for Enter key
    } else if (strcmp(token, "quit") == 0) {
        // Handle quit command
        exit(EXIT_SUCCESS);
    } else {
        // Not an internal command, execute as external command
        execute_external_command(command);
    }
}

// Function to execute external commands
void execute_external_command(char *command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        char *parent_shell_path = getenv("shell");
        setenv("parent", parent_shell_path, 1);
        execlp("sh", "sh", "-c", command, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wait(NULL);
    }
}

