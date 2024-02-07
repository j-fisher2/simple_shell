#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    set_shell_environment();

    if (argc > 1) {
        // Batch mode
        FILE *batchFile = fopen(argv[1], "r");
        if (!batchFile) {
            perror("Error opening batch file");
            exit(EXIT_FAILURE);
        }

        char command[256];
        while (fgets(command, sizeof(command), batchFile) != NULL) {
            // Remove newline character
            command[strcspn(command, "\n")] = 0;

            if (strlen(command) > 0) {
                if (strcmp(command, "quit") == 0) {
                    break;
                }

                printf("Executing command: %s\n", command);
                execute_internal_command(command);
            }
        }

        fclose(batchFile);
    } else {
        // Interactive mode
        char command[256];

        while (1) {
            printf("myshell> ");
            fgets(command, sizeof(command), stdin);

            // Remove newline character
            command[strcspn(command, "\n")] = 0;

            if (strcmp(command, "quit") == 0) {
                break;
            }

            execute_internal_command(command);
        }
    }

    return 0;
}

