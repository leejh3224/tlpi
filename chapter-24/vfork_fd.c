#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    switch(fork()) {
    case -1:
        errExit("fork");
    case 0:
        if (close(STDIN_FILENO) == -1)
            errExit("close");
        _exit(EXIT_SUCCESS);
    default:
        printf("(PID=%ld) parent can read stdin even after the child closed it\n", (long) getpid());

        if (fcntl(STDIN_FILENO, F_GETFD) == -1)
            errExit("fcntl - F_GETFD");
        break;
    }
}
