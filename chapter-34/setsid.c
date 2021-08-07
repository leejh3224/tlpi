#include <unistd.h>
#include "tlpi_hdr.h"

// This program throws only if it was ran directly from the shell
// You can do it via `docker run -it --entrypoint /bin/bash tlpi && ./exe/out`
// If you just run `./run.sh` then PGID/PID becoems different
int main(int argc, char *argv[]) {
    printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n", (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));

    if (setsid() == -1) {
        errExit("setsid");
    }
    
    printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n", (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));

    exit(EXIT_SUCCESS);
}
