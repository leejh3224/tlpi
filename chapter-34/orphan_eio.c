#include <unistd.h>
#include <stdio.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    switch (fork()) {
        case -1: errExit("fork");
        case 0:
            sleep(1);
            if (read(STDIN_FILENO, NULL, 0) == -1)
                errExit("read");
            break;
        default:
            _exit(EXIT_SUCCESS);
            break;
    }
}
