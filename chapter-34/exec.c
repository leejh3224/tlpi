#include <stdio.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, const char *argv[]){
    pid_t child;

    switch(child = fork()){
        case -1:
            errExit("fork");
        case 0:
            if (setpgid(0, 0) == -1)
                errExit("setpgid: child");
            printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n", (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));
            execl("/bin/ls", "ls", "-l", NULL);
            break;
        default:

            // wait for child to finish exec
            // comment this to make this program not throw
            if (wait(NULL) == -1) {
                errExit("wait");
            } 

            // both child and parent should have this line because the order of execution between them is not guaranteed to have specific order
            if (setpgid(child, 0) == -1 && errno != EACCES)
                errExit("setpgid: parent");

            break;
    }

    exit(EXIT_SUCCESS);
}