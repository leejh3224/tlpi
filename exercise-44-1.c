#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "tlpi_hdr.h"
#include "st_handler.h"
#include <string.h>

#define BUF_SIZE 50

int is_empty(const char *s) {
    while (isspace(*s) && s++);
    return !*s;
}

void to_upper(char *s) {
    while (*s) {
        *s = toupper(*s);
        s++;
    }
}

int main(int argc, char *argv[]) {

    // stack trace for debugging
    if (signal(SIGSEGV, &st_handler) == SIG_ERR)
        errExit("signal");

    // to avoid interleaving of stdout messages
    setbuf(stdout, NULL);

    for (;;) {

        /*
         * pipe1: parent -> child
         * pipe2: child -> parent
         */
        int pipe1[2], pipe2[2];
        char buf[BUF_SIZE];

        if (pipe(pipe1) == -1)
            errExit("pipe");

        if (pipe(pipe2) == -1)
            errExit("pipe");
        
        printf("input: ");

        if (fgets(buf, BUF_SIZE, stdin) == NULL)
            break;

        if (is_empty(buf)) {
            printf("input string is empty. Please try again!\n");
            continue;
        }

        buf[strlen(buf) - 1] = '\0';

        switch (fork()) {
            case -1: errExit("fork"); break;
            case 0:

                // close unused pipes
                if (close(pipe1[1]) == -1)
                    errExit("close");
                
                if (close(pipe2[0]) == -1)
                    errExit("close");

                if (read(pipe1[0], buf, strlen(buf)) != strlen(buf)) {
                    errExit("failed read - child");
                }
                
                to_upper(buf);

                if (write(pipe2[1], buf, strlen(buf)) != strlen(buf))
                    errExit("failed write - child");

                if (close(pipe1[0]) == -1)
                    errExit("close");
                
                if (close(pipe2[1]) == -1)
                    errExit("close");

                _exit(EXIT_SUCCESS);
            default:
                if (close(pipe1[0]) == -1)
                    errExit("close");
                
                if (close(pipe2[1]) == -1)
                    errExit("close");

                if (write(pipe1[1], buf, strlen(buf)) != strlen(buf))
                    errExit("failed write - parent");

                if (read(pipe2[0], buf, strlen(buf)) != strlen(buf)) {
                    errExit("failed read - parent");
                }
       
                if (close(pipe1[1]) == -1)
                    errExit("close");

                if (close(pipe2[0]) == -1)
                    errExit("close");

                fprintf(stdout, "echo: %s\n", buf);
        }
    }
}
