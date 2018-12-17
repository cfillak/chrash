#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "log.h"

log_t Log;

int main() {

    ssize_t char_ct;
    size_t line_size = 0;
    char *line = NULL;
    char *CEP = "Command executed by pid=%d\n";

    log_init(&Log);

    while (1) {
        pid_t pid = getpid();
        char *CWD = getcwd(0,0);
        printf("(pid=%d)%s$ ", pid, CWD);
        free(CWD);
        fflush(stdout);

        char_ct = getline(&line, &line_size, stdin);
        line[--char_ct] = '\0';

        if (char_ct <= 0) {
            continue;
        } else {
            char *command[500] = {NULL};
            char *t;
            char delimiters[] = " ";
            char *line_temp = line;

            int j = 0;
            while ( (t = strsep(&line_temp, delimiters)) ) {
                command[j] = t;
                j++;
            }

            log_push(&Log, command[0]);

            /* BEGIN BUILT IN FUNCTIONS */

            if (!strncmp(command[0], "cd", 2)) {
                if (chdir(command[1]) == -1) {
                    printf("%s: No such file or directory\n", command[1]);
                }
                continue;
            } else if (!strncmp(command[0], "exit", 4)) {
                free(line);
                log_destroy(&Log);
                break;
            }

            /* END BUILT IN FUNCTIONS */

            pid_t child_pid, cpid;
            switch (child_pid = fork()) {
                case -1:                            // fork() failure
                    printf("Fork Failed\n");
                    exit(1);
                case 0:                             // Child of successful fork()
                    cpid = getpid();
                    execvp(command[0], command);    // Execute all non built in commands
                    printf("%s: not found\n", line);
                    exit(1);

                default:                        // Parent process
                    waitpid(child_pid, NULL, WUNTRACED);
            }
        }
    }
    return 0;
}
