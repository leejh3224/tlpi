#include <sys/time.h>
#include <stdio.h>
#include "tlpi_hdr.h"

unsigned int alarm(unsigned int seconds) {
    struct itimerval old_val;
    struct itimerval new_val;

    new_val.it_value.tv_sec = seconds;
    new_val.it_value.tv_usec = 0;
    new_val.it_interval.tv_sec = 0;
    new_val.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &new_val, &old_val) == -1)
        errExit("setitimer");

    return old_val.it_value.tv_sec;
}

int main() {
    alarm(2);
}
